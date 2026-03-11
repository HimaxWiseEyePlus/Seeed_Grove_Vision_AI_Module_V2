/*
 * cvapp.cpp
 *
 *  Created on: 2018�~12��4��
 *      Author: 902452
 */

#include <cstdio>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "WE2_device.h"
#include "board.h"
#include "cvapp_mb_cls.h"
#include "cisdp_sensor.h"

#include "WE2_core.h"

#include "ethosu_driver.h"

#include <executorch/runtime/core/memory_allocator.h>
#include <executorch/runtime/executor/program.h>
#include <executorch/runtime/platform/log.h>
#include <executorch/runtime/platform/platform.h>
#include <executorch/runtime/platform/runtime.h>
#include <executorch/runtime/executor/method.h>
#include <executorch/runtime/core/data_loader.h>
#include <executorch/runtime/executor/memory_manager.h> 
#include <executorch/extension/data_loader/buffer_data_loader.h>
#include <memory>
#include "arm_memory_allocator.h"
#include "img_proc_helium.h"


#include "xprintf.h"
#include "spi_master_protocol.h"
#include "cisdp_cfg.h"
#include "memory_manage.h"
#include <send_result.h>
#include <forward_list>
#include "common_config.h"
#define INPUT_IMAGE_CHANNELS 3


#define MB_CLS_INPUT_TENSOR_WIDTH   224
#define MB_CLS_INPUT_TENSOR_HEIGHT  224
#define MB_CLS_INPUT_TENSOR_CHANNEL INPUT_IMAGE_CHANNELS


#define MB_CLS_DBG_APP_LOG             (1)


#if MB_CLS_DBG_APP_LOG
    #define dbg_cv_log(fmt, ...)       xprintf(fmt, ##__VA_ARGS__)
#else
    #define dbg_cv_log(fmt, ...)
#endif

#if UINT_TEST
#include "img_pattern.h"
#endif
// #define EACH_STEP_TICK
#define TOTAL_STEP_TICK

uint32_t systick_1, systick_2;
uint32_t loop_cnt_1, loop_cnt_2;
#define CPU_CLK	0xffffff+1
static uint32_t capture_image_tick = 0;
#ifdef TRUSTZONE_SEC
#define U55_BASE	BASE_ADDR_APB_U55_CTRL_ALIAS
#else
#ifndef TRUSTZONE
#define U55_BASE	BASE_ADDR_APB_U55_CTRL_ALIAS
#else
#define U55_BASE	BASE_ADDR_APB_U55_CTRL
#endif
#endif

using executorch::aten::ScalarType;
using executorch::aten::Tensor;
using executorch::runtime::Error;
using executorch::runtime::EValue;
using executorch::runtime::HierarchicalAllocator;
using executorch::runtime::MemoryAllocator;
using executorch::runtime::MemoryManager;
using executorch::runtime::Method;
using executorch::runtime::MethodMeta;
using executorch::runtime::Program;
using executorch::runtime::Result;
using executorch::runtime::Span;
using executorch::runtime::Tag;
using executorch::runtime::TensorInfo;

using namespace torch::executor;

using namespace std;

namespace {
#if TEST_SAMLL_MODEL
constexpr int method_allocation_pool_size = 60*1024;
static uint32_t method_allocation_pool=0;//

constexpr int temp_allocation_pool_size = 2*1024;//temp_allocation_pool_size is the same concept as tensor arena size
static uint32_t temp_allocation_pool=0;//temp_allocation_pool is the same concept as tensor arena
#else
#ifdef __ARM__
//only for mv2_quant_himax_ini_opt_size_vela_4_5_0_inout_in8.pte (int8 in out model)
//the SRAM is `not` enough for mv2_quant_himax_ini_opt_size_vela_4_5_0.pte (floating in out model)
constexpr int method_allocation_pool_size = 200*1024;//300*1024; //736*1024;
#else
//the SRAM is enough for mv2_quant_himax_ini_opt_size_vela_4_5_0.pte (floating in out model)
constexpr int method_allocation_pool_size = 736*1024;
#endif
static uint32_t method_allocation_pool=0;//

constexpr int temp_allocation_pool_size = 897*1024;//temp_allocation_pool_size is the same concept as tensor arena size
static uint32_t temp_allocation_pool=0;//temp_allocation_pool is the same concept as tensor arena
#endif
struct ethosu_driver ethosu_drv; /* Default Ethos-U device driver */
 const char* method_name = nullptr;
};



/// Lightweight heapless container that constructs and stores a T in-place.
template <typename T>
class Box {
 public:
  Box() = default;

  ~Box() {
    if (has_value) {
      ptr()->~T();
    }
  }

  Box(const Box&) = delete;
  Box& operator=(const Box&) = delete;

  /// Destructs the already contained object if it's present and initialize a
  /// new contained object while forwarding its constructor arguments.
  template <typename... Args>
  void reset(Args&&... args) {
    if (has_value) {
      // Destroy the already contained object.
      reinterpret_cast<T*>(mem)->~T();
    }
    // Init the new object.
    new (mem) T(std::forward<Args>(args)...);
    has_value = true;
  }

  /// Returns a reference to the contained object.
  T& value() {
    return *ptr();
  }

  /// Returns a const reference to the contained object.
  const T& value() const {
    return *ptr();
  }

  T* operator->() {
    return ptr();
  }

  const T* operator->() const {
    return ptr();
  }

 private:
  alignas(T) uint8_t mem[sizeof(T)];
  bool has_value = false;

  T* ptr() {
    return reinterpret_cast<T*>(mem);
  }

  const T* ptr() const {
    return reinterpret_cast<const T*>(mem);
  }
};

/// Holds all state needed for setup and run phases
struct RunnerContext {
  RunnerContext() = default;
  RunnerContext(const RunnerContext& ctx) = delete;
  RunnerContext& operator=(const RunnerContext& ctx) = delete;

  const char* method_name = nullptr;
  size_t planned_buffer_memsize = 0;
  size_t method_loaded_memsize = 0;
  size_t executor_membase = 0;
  size_t program_data_len = 0;
  size_t input_memsize = 0;
  size_t pte_size = 0;
  bool bundle_io = false;
  Box<executorch::extension::BufferDataLoader> data_loader;
  Box<Result<Program>> program;
  Box<MemoryAllocator> method_allocator;
  Box<MemoryAllocator> temp_allocator;
  Box<HierarchicalAllocator> planned_memory;
  Box<MemoryManager> memory_manager;
  std::vector<Span<uint8_t>> planned_spans;
  std::vector<uint8_t*> planned_buffers;
  Box<Result<Method>> method;
};

  RunnerContext ctx;
static void _arm_npu_irq_handler(void)
{
    /* Call the default interrupt handler from the NPU driver */
    ethosu_irq_handler(&ethosu_drv);
}

/**
 * @brief  Initialises the NPU IRQ
 **/
static void _arm_npu_irq_init(void)
{
    const IRQn_Type ethosu_irqnum = (IRQn_Type)U55_IRQn;

    /* Register the EthosU IRQ handler in our vector table.
     * Note, this handler comes from the EthosU driver */
    EPII_NVIC_SetVector(ethosu_irqnum, (uint32_t)_arm_npu_irq_handler);

    /* Enable the IRQ */
    NVIC_EnableIRQ(ethosu_irqnum);

}

static int _arm_npu_init(bool security_enable, bool privilege_enable)
{
    int err = 0;

    /* Initialise the IRQ */
    _arm_npu_irq_init();

    /* Initialise Ethos-U55 device */
	void * const ethosu_base_address = (void *)(U55_BASE);

    if (0 != (err = ethosu_init(
                            &ethosu_drv,             /* Ethos-U driver device pointer */
                            ethosu_base_address,     /* Ethos-U NPU's base address. */
                            NULL,       /* Pointer to fast mem area - NULL for U55. */
                            0, /* Fast mem region size. */
							security_enable,                       /* Security enable. */
							privilege_enable))) {                   /* Privilege enable. */
    	xprintf("failed to initalise Ethos-U device\n");
            return err;
        }

    xprintf("Ethos-U55 device initialised\n");

    return 0;
}



void et_pal_init(void) {
  // Enable ARM PMU Clock
  ARM_PMU_Enable();
  DCB->DEMCR |= DCB_DEMCR_TRCENA_Msk; // Trace enable
  ARM_PMU_CYCCNT_Reset();
  ARM_PMU_CNTR_Enable(PMU_CNTENSET_CCNTR_ENABLE_Msk);
}

/**
 * Implementation of the et_pal_<funcs>()
 *
 * This functions are hardware adaption type of functions for things like
 * time/logging/memory allocation that could call your RTOS or need to to
 * be implemnted in some way.
 */

ET_NORETURN void et_pal_abort(void) {
#if !defined(SEMIHOSTING)
  __builtin_trap();
#else
  _exit(-1);
#endif
}

et_timestamp_t et_pal_current_ticks(void) {
  return ARM_PMU_Get_CCNTR();
}

et_tick_ratio_t et_pal_ticks_to_ns_multiplier(void) {
  // Since we don't know the CPU freq for your target and justs cycles in the
  // FVP for et_pal_current_ticks() we return a conversion ratio of 1
  return {1, 1};
}

/**
 * Emit a log message via platform output (serial port, console, etc).
 */
void et_pal_emit_log_message(
    ET_UNUSED et_timestamp_t timestamp,
    et_pal_log_level_t level,
    const char* filename,
    ET_UNUSED const char* function,
    size_t line,
    const char* message,
    ET_UNUSED size_t length) {
  fprintf(
      stderr,
      "%c [executorch:%s:%lu %s()] %s\n",
      level,
      filename,
      line,
      function,
      message);
}

/**
 * Dynamic memory allocators intended to be used by temp_allocator
 * to implement malloc()/free() type of allocations.
 * Currenyly not used.
 */

void* et_pal_allocate(ET_UNUSED size_t size) {
  return nullptr;
}

void et_pal_free(ET_UNUSED void* ptr) {}



extern void executorch_ethosu_force_link(void);
extern void RegisterCodegenUnboxedKernelsEverything_force_link(void);
int cv_mb_cls_init(bool security_enable, bool privilege_enable, uint32_t model_addr, uint32_t model_size) {
	int ercode = 0;
	executorch_ethosu_force_link();
	RegisterCodegenUnboxedKernelsEverything_force_link();
	if(_arm_npu_init(security_enable, privilege_enable)!=0)
			return -1;

	ctx.data_loader.reset((const void *)model_addr, model_size);
	ctx.program.reset(Program::load(&ctx.data_loader.value()));

	if (!ctx.program.value().ok()) {
		dbg_cv_log(
			"Program loading failed @ 0x%p: 0x%x\r\n",
			model_addr,
			(unsigned int)ctx.program.value().error());
	}


    dbg_cv_log("Model buffer loaded, has %lu methods\r\n", ctx.program.value()->num_methods());






	const auto method_name_result = ctx.program.value()->get_method_name(0);
	ET_CHECK_MSG(method_name_result.ok(), "Program has no methods");
	ctx.method_name = *method_name_result;

	dbg_cv_log("Running method %s\r\n", ctx.method_name);


	Result<MethodMeta> method_meta = ctx.program.value()->method_meta(ctx.method_name);
	if (!method_meta.ok()) {
		dbg_cv_log(
			"Failed to get method_meta for %s: 0x%x\r\n",
			ctx.method_name,
			(unsigned int)method_meta.error());
	}

	size_t num_inputs = method_meta->num_inputs();
	dbg_cv_log("Method has %lu inputs.\r\n", num_inputs);
	for (size_t i = 0; i < num_inputs; i++) {
		auto tag_result = method_meta->input_tag(i);
		if (tag_result.ok()) {
			dbg_cv_log("Input %lu tag: %d\r\n", i, (int)tag_result.get());
		} else {
			dbg_cv_log("Input %lu tag error\r\n", i);
		}
		auto tensor_meta = method_meta->input_tensor_meta(i);
		if (tensor_meta.ok()) {
			dbg_cv_log("Input %lu sizes: [", i);
			for (auto dim : tensor_meta->sizes()) {
				dbg_cv_log("%d, ", (int)dim);
			}
			dbg_cv_log("] Type: %d\r\n", (int)tensor_meta->scalar_type());
		}
	}

	size_t num_outputs = method_meta->num_outputs();
	dbg_cv_log("Method has %lu outputs.\r\n", num_outputs);
	for (size_t i = 0; i < num_outputs; i++) {
		auto tag_result = method_meta->output_tag(i);
		if (tag_result.ok()) {
			dbg_cv_log("Output %lu tag: %d\r\n", i, (int)tag_result.get());
		} else {
			dbg_cv_log("Output %lu tag error\r\n", i);
		}
		auto tensor_meta = method_meta->output_tensor_meta(i);
		if (tensor_meta.ok()) {
			dbg_cv_log("Output %lu sizes: [", i);
			for (auto dim : tensor_meta->sizes()) {
				dbg_cv_log("%d, ", (int)dim);
			}
			dbg_cv_log("] Type: %d\r\n", (int)tensor_meta->scalar_type());
		}
	}


	dbg_cv_log("Setup Method allocator pool. Size: %lu bytes.\r\n",
		method_allocation_pool_size);

	size_t num_memory_planned_buffers = method_meta->num_memory_planned_buffers();


	// REMOVED: method_allocation_pool = mm_reserve_align(method_allocation_pool_size,0x20);
	// Instead, we track total planned size to calculate remaining for method_allocator

	size_t total_planned_size = 0;

	// Use mm_reserve_align to allocate from the managed pool
	for (size_t id = 0; id < num_memory_planned_buffers; ++id) {
		size_t buffer_size =
			static_cast<size_t>(method_meta->memory_planned_buffer_size(id).get());
		dbg_cv_log( "Setting up planned buffer %lu, size %lu.\r\n", id, buffer_size);

		// Reserve aligned memory from the pool manager
		uint32_t buf_addr = mm_reserve_align(buffer_size, 32); 
		
		// Check if allocation failed
		if (buf_addr == 0) {
			xprintf("Error: mm_reserve_align failed for planned buffer %lu\r\n", id);
			return -1;
		}
		
		// Track usage (approximation, does not account for padding in total count but good enough for logic flow if we use remaining)
		// Actually, to get the TRUE remaining size for the method allocator, we should trust the pool manager or calculate simple subtraction if we knew the total available.
		// The user code seems to rely on `method_allocation_pool_size` as a constant for the TOTAL pool?
		
		// Let's assume we want to give the REST of the pool to the method allocator.
		// We can assume `method_allocation_pool_size` is the max.
		// We should track how much we used?
		// Better yet, just allocate logical blocks.
		
		uint8_t* buffer = reinterpret_cast<uint8_t*>(buf_addr);

		ctx.planned_buffers.push_back(buffer);
		ctx.planned_spans.push_back({ctx.planned_buffers.back(), buffer_size});

		dbg_cv_log("planned_buffers[%d]=0x%x  buffer_size: %d bytes\r\n",id,ctx.planned_buffers[id],buffer_size);
		
		// We need to account for alignment padding in our tracking if we want to be precise, 
		// but `reserved_size` isn't returned by `mm_reserve_align`.
		// Let's increment by requested size for now (or assume 32-byte alignment overhead is handled by MM)
		total_planned_size += buffer_size;
		// Align up for our own tracking to be safe?
		if (total_planned_size % 32 != 0) total_planned_size += (32 - (total_planned_size % 32));
	}

	/////////////////////////////////////////////////
	temp_allocation_pool =  mm_reserve_align(temp_allocation_pool_size,0x20);


	dbg_cv_log("Setting up temporary allocator pool. Base: %x, size: %lu bytes.\n",
			temp_allocation_pool, temp_allocation_pool_size);


	ctx.temp_allocator.reset(temp_allocation_pool_size, (uint8_t *)temp_allocation_pool);


	// Calculate remaining size for method_allocator
	// We subtract the ROUGHLY planned size from the TOTAL desired method pool size.
	// This assumes `method_allocation_pool_size` is the total capacity we have available/wanted for everything (buffers + scratch).
	size_t remaing_method_pool_size = method_allocation_pool_size - total_planned_size;

	// Now allocate the big chunk for the method allocator
	uint32_t method_allocator_addr = mm_reserve_align(remaing_method_pool_size, 32);

	if (method_allocator_addr == 0) {
		xprintf("Error: Failed to allocate method_allocator pool (size %lu)\r\n", remaing_method_pool_size);
		return -1;
	}

	dbg_cv_log("Corrected Allocation: method_allocator start: 0x%x, size: %lu\r\n", 
		method_allocator_addr, remaing_method_pool_size);

	ctx.method_allocator.reset(remaing_method_pool_size, (uint8_t *)method_allocator_addr);

	ctx.planned_memory.reset(
		Span<Span<uint8_t>>{ctx.planned_spans.data(), ctx.planned_spans.size()});

	// Assemble all of the allocators into the MemoryManager that the Executor
	// will use.
	ctx.memory_manager.reset(&ctx.method_allocator.value(), &ctx.planned_memory.value(), &ctx.temp_allocator.value());
	dbg_cv_log("memory_manager!~~\r\nmemory_manager.method_allocator: 0x%x\r\nmemory_manager.temp_allocator: 0x%x\r\n\n",
			ctx.memory_manager.value().method_allocator()->base_address(),ctx.memory_manager.value().temp_allocator()->base_address());


		executorch::runtime::EventTracer* eventTracerPtr = nullptr;
	//
	// Load the method from the program, using the provided allocators. Running
	// the method can mutate the memory-planned buffers, so the method should only
	// be used by a single thread at at time, but it can be reused.
	//
	ctx.method.reset(ctx.program.value()->load_method(ctx.method_name, &ctx.memory_manager.value(),eventTracerPtr));
	if(!ctx.method->ok())
	{
		dbg_cv_log(
			"Loading of method %s failed with status 0x%lu\r\n",
			ctx.method_name,
			(uint32_t)ctx.method->error());
	}
	////////////////////////////////////////////////////
	dbg_cv_log("ctx.method.value(): 0x%x\r\n",&ctx.method.value());

	dbg_cv_log("initial done\n");
	return ercode;
}

void print_float(float f_z)
{
	float o_f_z = f_z;
	short D_z, D_f_z;
	D_z = (short)f_z;
	f_z -=  D_z;

	if(f_z <=0)
		f_z = 0-f_z;

	D_f_z = f_z*1000;
	if(D_z==0 && o_f_z < 0)dbg_cv_log("-%d.%03d\r\n",D_z, D_f_z);
	else dbg_cv_log("%d.%03d\r\n",D_z, D_f_z);
}

int cv_mb_cls_run(struct_yolov8_ob_algoResult *algoresult_yolov8n_ob) {
	int ercode = 0;
    float w_scale;
    float h_scale;
    uint32_t img_w = app_get_raw_width();
    uint32_t img_h = app_get_raw_height();
    uint32_t ch = app_get_raw_channels();
    uint32_t raw_addr = app_get_raw_addr();
/////////////////////test here
dbg_cv_log("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\r\n");
// /////////////////////////////////////////////////
#if TEST_SAMLL_MODEL
int32_t data[5] = {1,5,10,15,25};
Tensor::SizesType sizes[] = {5};

// 定義維度順序 (Dim Order): 標準 NCHW 通常是 {0, 1, 2, 3}
Tensor::DimOrderType dim_order[] = {0};

#if 1
//////use exist plan buffer to copy input data
// 1. Get the pointer to the safe, reserved memory in the pool
void* input_ptr = ctx.planned_buffers[0]; 
memcpy(input_ptr, data, sizeof(data)); 
// 2. Wrap THAT pointer in a Tensor
TensorImpl impl(ScalarType::Int, 1, sizes, input_ptr, dim_order, nullptr, TensorShapeDynamism::STATIC);
#else
// ==========================================
// 步驟 2: 建立 TensorImpl (核心實作層)
// ==========================================

// TensorImpl 是一個輕量級的結構，用來描述數據在哪裡、長怎樣。
// 這裡最關鍵的是把 my_image_ptr 傳進去。
TensorImpl impl(
    ScalarType::Int, // <--- 資料型態 (Byte=UInt8, Char=Int8, Float=Float)
    1,                                     // <--- 維度數量 (Rank)
    sizes,                                 // <--- 形狀陣列
    data,                          // <--- 【關鍵】你的 Pointer 直接放這裡
    dim_order,                             // <--- 維度順序
    nullptr,                               // <--- Strides (填 nullptr 代表記憶體是連續的)
    TensorShapeDynamism::STATIC // 靜態形狀
);

// ==========================================
// 步驟 3: 建立 Tensor 並傳入
// ==========================================
#endif
// 建立 Tensor 包裝器 (指向上面的 impl)
Tensor t(&impl);
// Implicitly casts t to EValue
Error set_input_error = ctx.method.value()->set_input(t, 0);
assert(set_input_error == Error::Ok);


    // Verify input IS a tensor
    EValue input_val = ctx.method.value()->get_input(0);
    if (!input_val.isTensor()) {
        dbg_cv_log("FATAL: Input 0 is NOT a tensor after set_input!\r\n");
    } else {
        dbg_cv_log("Input 0 verified as Tensor.\r\n");
    }


    // DEBUG: Check inputs
    auto meta = ctx.method.value()->method_meta();
    size_t num_inputs = meta.num_inputs();
    if (num_inputs > 1) {
        dbg_cv_log("CRITICAL ERROR: Model has %lu inputs, but only input 0 was set! This will crash.\r\n", num_inputs);
         for (size_t i=0; i<num_inputs; ++i) {
            auto tag = meta.input_tag(i);
            dbg_cv_log("Input %lu tag: %d\r\n", i, (int)(tag.ok() ? tag.get() : Tag::None));
        }
    } else {
        dbg_cv_log("Model has 1 input. Input 0 set successfully.\r\n");
    }

	/////// do the inference
    Error exec_status = ctx.method.value()->execute();
    if (exec_status != Error::Ok) {
		dbg_cv_log("method.value()->execute fail\r\n");
		
      return 0;
    }

    // Print Results
    dbg_cv_log("Inference Results:\r\n");
    EValue output_val = ctx.method.value()->get_output(0);
    if (output_val.isTensor()) {
        Tensor t_out = output_val.toTensor();
        const int32_t* output_data_ptr = t_out.const_data_ptr<int32_t>();
        for(int i=0; i<5; ++i) {
            dbg_cv_log("  Class %d: %d\r\n", i, (int)output_data_ptr[i]);
        }
    } else {
        dbg_cv_log("Error: Output 0 is not a tensor! Tag: %d\r\n", (int)output_val.tag);
    }


#else
//////////////////////////////////
// 請依照你的模型需求修改，例如 {1, 224, 224, 3} 或 {1, 3, 224, 224}
Tensor::SizesType sizes[] = {1, 3, 224, 224};

// 定義維度順序 (Dim Order): 標準 NCHW 通常是 {0, 1, 2, 3}
Tensor::DimOrderType dim_order[] = {0, 1, 2, 3};



//////use exist plan buffer to copy input data
// 1. Get the pointer to the safe, reserved memory in the pool
void* input_ptr = ctx.planned_buffers[0]; 
#if UINT_TEST
  #if TEST_RESIZE_FROM_320_240_3
      img_w = 320;
      img_h = 240;
      w_scale = (float)(img_w - 1) / (MB_CLS_INPUT_TENSOR_WIDTH - 1);
      h_scale = (float)(img_h - 1) / (MB_CLS_INPUT_TENSOR_HEIGHT - 1);

      // Resize to model dimensions (always outputs uint8 BGR first)
      hx_lib_image_resize_helium((uint8_t*)org_image, (uint8_t*)input_ptr,  
                img_w, img_h, ch, 
                MB_CLS_INPUT_TENSOR_WIDTH, MB_CLS_INPUT_TENSOR_HEIGHT, w_scale,h_scale);

      // Detect Model Input Type
      auto input_meta = ctx.method.value()->method_meta().input_tensor_meta(0);
      ScalarType input_type = ScalarType::Char; // Default
      if (input_meta.ok()) {
          input_type = input_meta->scalar_type();
      } else {
          dbg_cv_log("Warning: Could not get input tensor meta. Defaulting to Char (Int8).\r\n");
      }

      dbg_cv_log("Detected Input ScalarType: %d\r\n", (int)input_type);

      if (input_type == ScalarType::Float) {
          // Preprocessing for FLOAT: BGR(uint8) -> RGB(float) + Normalize [0, 1]
          // Note: input_ptr currently holds uint8 data. We need to expand it to float.
          // CAUTION: Float takes 4x space. Ensure planned_buffer size is sufficient!
          // Since input_ptr points to the SAME buffer we are writing to, we must be careful not to overwrite data we haven't read yet if expanding.
          // Expanding 1 byte -> 4 bytes requires working BACKWARDS if in-place, 
          // BUT 'hx_lib_image_resize_helium' wrote to the BEGINNING of the buffer.
          // If the buffer is large enough for the full float image, we can work backwards.
          
          int total_pixels = MB_CLS_INPUT_TENSOR_WIDTH * MB_CLS_INPUT_TENSOR_HEIGHT;
          uint8_t* u8_src = (uint8_t*)input_ptr;
          float* f32_dst = (float*)input_ptr; // Re-use same buffer
          float meanVal[3] = {0.485, 0.456, 0.406};
          float stdVal[3] = {0.229, 0.224, 0.225};

          // Work backwards to avoid overwriting source data while expanding
          // AND convert HWC (Interleaved) -> NCHW (Planar)
          // Destination Indices:
          // R: 0 * total_pixels + i
          // G: 1 * total_pixels + i
          // B: 2 * total_pixels + i
          for (int i = total_pixels - 1; i >= 0; --i) {
              
              // Read BGR (uint8)
              uint8_t b_u8 = u8_src[0 * total_pixels + i];
              uint8_t g_u8 = u8_src[1 * total_pixels + i];
              uint8_t r_u8 = u8_src[2 * total_pixels + i];
            
              // Write RGB (float) normalized [0.0, 1.0] to PLANAER offsets
              // Red Plane
              f32_dst[0 * total_pixels + i] = ((float)r_u8 / 255.0f - meanVal[0]) / stdVal[0];
              // Green Plane
              f32_dst[1 * total_pixels + i] = ((float)g_u8 / 255.0f - meanVal[1]) / stdVal[1];
              // Blue Plane
              f32_dst[2 * total_pixels + i] = ((float)b_u8 / 255.0f - meanVal[2]) / stdVal[2];
          }
      } else {
          // Preprocessing for INT8: BGR(uint8) -> RGB(int8) + Subtract 128
          int8_t* s8_dst = (int8_t*)input_ptr;
          uint8_t* u8_src = (uint8_t*)input_ptr;
          int total_pixels = MB_CLS_INPUT_TENSOR_WIDTH * MB_CLS_INPUT_TENSOR_HEIGHT;

          for (int i = 0; i < total_pixels; ++i) {

              uint8_t b = u8_src[0 * total_pixels + i];
              uint8_t g = u8_src[1 * total_pixels + i];
              uint8_t r = u8_src[2 * total_pixels + i];
              
              s8_dst[0 * total_pixels + i] = (int8_t)(r - 128); // R
              s8_dst[1 * total_pixels + i] = (int8_t)(g - 128); // G
              s8_dst[2 * total_pixels + i] = (int8_t)(b - 128); // B
          }
      }
  #else //TEST_RESIZE_FROM_320_240_3
	memcpy(input_ptr, org_image,  MB_CLS_INPUT_TENSOR_WIDTH * MB_CLS_INPUT_TENSOR_HEIGHT * MB_CLS_INPUT_TENSOR_CHANNEL); 

	// Detect Model Input Type
	auto input_meta = ctx.method.value()->method_meta().input_tensor_meta(0);
	ScalarType input_type = ScalarType::Char; // Default
	if (input_meta.ok()) {
		input_type = input_meta->scalar_type();
	} else {
		dbg_cv_log("Warning: Could not get input tensor meta. Defaulting to Char (Int8).\r\n");
	}

	dbg_cv_log("Detected Input ScalarType: %d\r\n", (int)input_type);

	if (input_type == ScalarType::Float) {
		int total_pixels = MB_CLS_INPUT_TENSOR_WIDTH * MB_CLS_INPUT_TENSOR_HEIGHT;
		uint8_t* u8_src = (uint8_t*)input_ptr;
		float* f32_dst = (float*)input_ptr; // Re-use same buffer
    float meanVal[3] = {0.485, 0.456, 0.406};
    float stdVal[3] = {0.229, 0.224, 0.225};
		for (int i = total_pixels - 1; i >= 0; --i) {
      int idx = i * 3;
			// Read RGB (uint8)
			uint8_t r_u8 = u8_src[idx + 0];
			uint8_t g_u8 = u8_src[idx + 1];
			uint8_t b_u8 = u8_src[idx + 2];
    
      // Write RGB (float) normalized [0.0, 1.0] to PLANAER offsets
      // Red Plane
      f32_dst[0 * total_pixels + i] = ((float)r_u8 / 255.0f - meanVal[0]) / stdVal[0];
      // Green Plane
      f32_dst[1 * total_pixels + i] = ((float)g_u8 / 255.0f - meanVal[1]) / stdVal[1];
      // Blue Plane
      f32_dst[2 * total_pixels + i] = ((float)b_u8 / 255.0f - meanVal[2]) / stdVal[2];
		}
	} else {
		// Preprocessing for INT8: BGR(uint8) -> RGB(int8) + Subtract 128
		int8_t* s8_dst = (int8_t*)input_ptr;
		uint8_t* u8_src = (uint8_t*)input_ptr;
		int total_pixels = MB_CLS_INPUT_TENSOR_WIDTH * MB_CLS_INPUT_TENSOR_HEIGHT;

		for (int i = 0; i < total_pixels; ++i) {
			int idx = i * 3;
			uint8_t r = u8_src[idx + 0];
			uint8_t g = u8_src[idx + 1];
			uint8_t b = u8_src[idx + 2];
			
			s8_dst[i + 0 * total_pixels] = (int8_t)(r - 128); // R
			s8_dst[i + 1 * total_pixels] = (int8_t)(g - 128); // G
			s8_dst[i + 2 * total_pixels] = (int8_t)(b - 128); // B
		}
	}
  #endif//TEST_RESIZE_FROM_320_240_3
#else//NON UINT_TEST
w_scale = (float)(img_w - 1) / (MB_CLS_INPUT_TENSOR_WIDTH - 1);
h_scale = (float)(img_h - 1) / (MB_CLS_INPUT_TENSOR_HEIGHT - 1);

// Resize to model dimensions (always outputs uint8 BGR first)
hx_lib_image_resize_helium((uint8_t*)raw_addr, (uint8_t*)input_ptr,  
					img_w, img_h, ch, 
					MB_CLS_INPUT_TENSOR_WIDTH, MB_CLS_INPUT_TENSOR_HEIGHT, w_scale,h_scale);

// Detect Model Input Type
auto input_meta = ctx.method.value()->method_meta().input_tensor_meta(0);
ScalarType input_type = ScalarType::Char; // Default
if (input_meta.ok()) {
    input_type = input_meta->scalar_type();
} else {
    dbg_cv_log("Warning: Could not get input tensor meta. Defaulting to Char (Int8).\r\n");
}

dbg_cv_log("Detected Input ScalarType: %d\r\n", (int)input_type);

if (input_type == ScalarType::Float) {
    // Preprocessing for FLOAT: BGR(uint8) -> RGB(float) + Normalize [0, 1]
    // Note: input_ptr currently holds uint8 data. We need to expand it to float.
    // CAUTION: Float takes 4x space. Ensure planned_buffer size is sufficient!
    // Since input_ptr points to the SAME buffer we are writing to, we must be careful not to overwrite data we haven't read yet if expanding.
    // Expanding 1 byte -> 4 bytes requires working BACKWARDS if in-place, 
    // BUT 'hx_lib_image_resize_helium' wrote to the BEGINNING of the buffer.
    // If the buffer is large enough for the full float image, we can work backwards.
    
    int total_pixels = MB_CLS_INPUT_TENSOR_WIDTH * MB_CLS_INPUT_TENSOR_HEIGHT;
    uint8_t* u8_src = (uint8_t*)input_ptr;
    float* f32_dst = (float*)input_ptr; // Re-use same buffer
    float meanVal[3] = {0.485, 0.456, 0.406};
    float stdVal[3] = {0.229, 0.224, 0.225};

    // Work backwards to avoid overwriting source data while expanding
    // AND convert HWC (Interleaved) -> NCHW (Planar)
    // Destination Indices:
    // R: 0 * total_pixels + i
    // G: 1 * total_pixels + i
    // B: 2 * total_pixels + i
    for (int i = total_pixels - 1; i >= 0; --i) {
        
        // Read BGR (uint8)
        uint8_t b_u8 = u8_src[0 * total_pixels + i];
        uint8_t g_u8 = u8_src[1 * total_pixels + i];
        uint8_t r_u8 = u8_src[2 * total_pixels + i];
      
        // Write RGB (float) normalized [0.0, 1.0] to PLANAER offsets
        // Red Plane
        f32_dst[0 * total_pixels + i] = ((float)r_u8 / 255.0f - meanVal[0]) / stdVal[0];
        // Green Plane
        f32_dst[1 * total_pixels + i] = ((float)g_u8 / 255.0f - meanVal[1]) / stdVal[1];
        // Blue Plane
        f32_dst[2 * total_pixels + i] = ((float)b_u8 / 255.0f - meanVal[2]) / stdVal[2];
    }
} else {
    // Preprocessing for INT8: BGR(uint8) -> RGB(int8) + Subtract 128
    int8_t* s8_dst = (int8_t*)input_ptr;
    uint8_t* u8_src = (uint8_t*)input_ptr;
    int total_pixels = MB_CLS_INPUT_TENSOR_WIDTH * MB_CLS_INPUT_TENSOR_HEIGHT;

    for (int i = 0; i < total_pixels; ++i) {

        uint8_t b = u8_src[0 * total_pixels + i];
        uint8_t g = u8_src[1 * total_pixels + i];
        uint8_t r = u8_src[2 * total_pixels + i];
        
        s8_dst[0 * total_pixels + i] = (int8_t)(r - 128); // R
        s8_dst[1 * total_pixels + i] = (int8_t)(g - 128); // G
        s8_dst[2 * total_pixels + i] = (int8_t)(b - 128); // B
    }
}
#endif//UINT_TEST
// 2. Wrap THAT pointer in a Tensor
TensorImpl impl(input_type, 4, sizes, input_ptr, dim_order, nullptr, TensorShapeDynamism::STATIC);


// ==========================================
// 3. Setup Input
// ==========================================
Tensor t(&impl);
Error set_input_error = ctx.method.value()->set_input(t, 0);
assert(set_input_error == Error::Ok);


    Error exec_status = ctx.method.value()->execute();
    if (exec_status != Error::Ok) {
		dbg_cv_log("method.value()->execute fail\r\n");
        return 0;
    }

	// Print Results
    dbg_cv_log("Inference Results:\r\n");
    EValue output_val = ctx.method.value()->get_output(0);
    
    if (output_val.isTensor()) {
        Tensor t_out = output_val.toTensor();
        ScalarType out_type = t_out.scalar_type();
        dbg_cv_log("Output ScalarType: %d\r\n", (int)out_type);
        
        std::vector<std::pair<float, int>> scores;
        scores.reserve(1000);

        if (out_type == ScalarType::Float) {
            const float* output_data_ptr = t_out.const_data_ptr<float>();
            for(int i=0; i<1000; ++i) {
                scores.push_back({output_data_ptr[i], i});
            }
        } else {
            // Assume Int8/Char
            const int8_t* output_data_ptr = t_out.const_data_ptr<int8_t>();
            for(int i=0; i<1000; ++i) {
                scores.push_back({(float)output_data_ptr[i], i});
            }
        }
        
        // Sort descending
        std::sort(scores.begin(), scores.end(), [](const std::pair<float, int>& a, const std::pair<float, int>& b) {
            return a.first > b.first;
        });

        dbg_cv_log("Top 5 Results:\r\n");
        for(int i=0; i<5; ++i) {
            // Print as integer if it was int8 source, or float if float source. 
            // Simplified: just print as float for generic handling
            if (out_type == ScalarType::Float)
            {
              dbg_cv_log("  Class %d Score: ", scores[i].second);
              print_float(scores[i].first);
            } 

            else
                 dbg_cv_log("  Class %d (Score: %d)\r\n", scores[i].second, (int)scores[i].first);
        }
    } else {
        dbg_cv_log("Error: Output 0 is not a tensor! Tag: %d\r\n", (int)output_val.tag);
    }

#endif
////////////////////////////////////
dbg_cv_log("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\r\n");
/////////////////////////////////////////////////////

/***
 * open to recapture
 * ***/

	// SystemGetTick(&systick_1, &loop_cnt_1);
	// //recapture image
	sensordplib_retrigger_capture();

	
	// SystemGetTick(&systick_2, &loop_cnt_2);
	// capture_image_tick = (loop_cnt_2-loop_cnt_1)*CPU_CLK+(systick_1-systick_2);	
	return ercode;
}

int cv_mb_cls_deinit()
{
	
	return 0;
}

