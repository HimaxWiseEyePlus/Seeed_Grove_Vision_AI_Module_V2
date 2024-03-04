extern "C" {
#include <hx_drv_uart.h>
#include "hx_drv_swreg_aon.h"
#include "tflm_fd_fm.h"
}
#include <math.h>

#include <cstring>
#include <cstdint>
#include <forward_list>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include "WE2_core.h"
#include <send_result.h>

static char*       img_2_json_str_buffer      = nullptr;

el_err_code_t send_bytes(const char* buffer, size_t size) {
    DEV_UART* console_uart;
    console_uart = hx_drv_uart_get_dev((USE_DW_UART_E)CONSOLE_UART_ID);
    console_uart->uart_open(UART_BAUDRATE_921600);
    size_t sent{0};
    size_t pos_of_bytes{0};

    while (size) {
        size_t bytes_to_send{size < 8 ? size : 8};

        sent += console_uart->uart_write(buffer + pos_of_bytes, bytes_to_send);
        pos_of_bytes += bytes_to_send;
        size -= bytes_to_send;
    }

    //0 is ok(success)
    //1 is again (fail)
    return sent == pos_of_bytes ? EL_OK : EL_AGAIN;
}


el_err_code_t read_bytes(char* buffer, size_t size) {
    DEV_UART* console_uart;
    console_uart = hx_drv_uart_get_dev((USE_DW_UART_E)CONSOLE_UART_ID);
    console_uart->uart_open(UART_BAUDRATE_921600);

    size_t read{0};
    size_t pos_of_bytes{0};
    while (size) {
        size_t bytes_to_read{size < 8 ? size : 8};
        read += console_uart->uart_read(buffer + pos_of_bytes, bytes_to_read);

        pos_of_bytes += bytes_to_read;
        size -= bytes_to_read;
    }

    return read > 0 ? EL_OK : EL_AGAIN;
}

el_err_code_t read_bytes_nonblock(char* buffer, size_t size) {
    DEV_UART* console_uart;
    console_uart = hx_drv_uart_get_dev((USE_DW_UART_E)CONSOLE_UART_ID);
    console_uart->uart_open(UART_BAUDRATE_921600);

    size_t read{0};
    size_t pos_of_bytes{0};
    while (size) {
        size_t bytes_to_read{size < 8 ? size : 8};

        read += console_uart->uart_read_nonblock(buffer + pos_of_bytes, bytes_to_read);
        
        pos_of_bytes += bytes_to_read;
        size -= bytes_to_read;

    }

    return read > 0 ? EL_OK : EL_AGAIN;
}


void set_model_change_by_uart()
{
    char c{'\0'};
    if(!read_bytes_nonblock((char *)(&c), 1))//if read ok
    {
        uint32_t data;
        hx_drv_swreg_aon_get_appused1(&data);
        //transfer type
        uint16_t trans_type = (data>>16);
        //model case 
        uint16_t model_case = (data&0xff);

        if( c == 255)// set using UART (trans_type == 0)
        {
            hx_drv_swreg_aon_set_appused1( (0x0<<16) | (model_case&0xff)); 
            delete[] img_2_json_str_buffer;
            SetPSPDNoVid();
        }
        if( c == 254)// set using SPI (trans_type == 1)
        {
            hx_drv_swreg_aon_set_appused1( (0x1<<16) | (model_case&0xff)); 
            delete[] img_2_json_str_buffer;
            SetPSPDNoVid();
        }
        if( c == 253)// set using SPI & UART (trans_type == 2)
        {
            hx_drv_swreg_aon_set_appused1( (0x2<<16) | (model_case&0xff));
            delete[] img_2_json_str_buffer; 
            SetPSPDNoVid();
        }
        if(c == 0)
        {
            hx_drv_swreg_aon_set_appused1( (trans_type<<16) | (0 & 0xff));
            delete[] img_2_json_str_buffer;
            SetPSPDNoVid();
        }
        // if(c == 5)
        // {
        //     hx_drv_swreg_aon_set_appused1( (trans_type<<16) | (5 & 0xff));
        //     delete[] img_2_json_str_buffer;
        //     SetPSPDNoVid(); 
        // }
        // if(c == 6)
        // {
        //     hx_drv_swreg_aon_set_appused1( (trans_type<<16) | (6&0xff));
        //     delete[] img_2_json_str_buffer;
        //     SetPSPDNoVid(); 
        // }
        // if(c == 9)
        // {
        //    hx_drv_swreg_aon_set_appused1( (trans_type<<16) | (9&0xff));
        //     delete[] img_2_json_str_buffer;
        //     SetPSPDNoVid();
        // }
        // if(c == 10)
        // {
        //     hx_drv_swreg_aon_set_appused1( (trans_type<<16) | (10&0xff));
        //     delete[] img_2_json_str_buffer;
        //     SetPSPDNoVid();
        // }
        // if(c == 11)
        // {
        //     hx_drv_swreg_aon_set_appused1( (trans_type<<16) | (11&0xff));
        //     delete[] img_2_json_str_buffer;
        //     SetPSPDNoVid();
        // }
    }

}

	





static const char* BASE64_CHARS_TABLE = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
void el_base64_encode(const unsigned char* in, int in_len, char* out) {
    int           i = 0;
    int           j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    while (in_len--) {
        char_array_3[i++] = *(in++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (i = 0; i < 4; i++) *out++ = BASE64_CHARS_TABLE[char_array_4[i]];

            i = 0;
        }
    }

    if (i) {
        for (j = i; j < 3; j++) char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (j = 0; j < i + 1; j++) *out++ = BASE64_CHARS_TABLE[char_array_4[i]];

        while (i++ < 3) *out++ = '=';
    }
}

std::string img_2_json_str(el_img_t* img) {
if (!img || !img->data || !img->size) [[unlikely]]
        return std::string("\"image\": \"\"");

    static std::size_t size        = 0;
    static std::size_t buffer_size = 0;


    // only reallcate memory when buffer size is not enough
    if (img->size > size) [[unlikely]] {
        size        = img->size;
        buffer_size = (((size + 2u) / 3u) << 2u) + 1u;  // base64 encoded size, +1 for terminating null character
        printf("buffer_size: %d may be too small reallocating\r\n\n",buffer_size);
        printf("if still fail, please modify linker script heap size\r\n\n");
        if (img_2_json_str_buffer) [[likely]]
            delete[] img_2_json_str_buffer;
        img_2_json_str_buffer = new char[buffer_size]{};
    }

    std::memset(img_2_json_str_buffer, 0, buffer_size);
    el_base64_encode(img->data, img->size, img_2_json_str_buffer);

    return concat_strings("\"image\": \"", img_2_json_str_buffer, "\"");
}


std::string img_res_2_json_str(const el_img_t* img) {
    return concat_strings("\"resolution\": [", std::to_string(img->width), ", ", std::to_string(img->height), "]");
}



template <typename T> constexpr std::string  results_2_json_str(const std::forward_list<T>& results) {
    std::string ss;
    const char* delim = "";

    if (std::is_same<T, el_box_t>::value) {
        ss = "\"boxes\": [";
        for (const auto& box : results) {
            ss += concat_strings(delim,
                                 "[",
                                 std::to_string(box.x),
                                 ", ",
                                 std::to_string(box.y),
                                 ", ",
                                 std::to_string(box.w),
                                 ", ",
                                 std::to_string(box.h),
                                 ", ",
                                 std::to_string(box.score),
                                 ", ",
                                 std::to_string(box.target),
                                 "]");
            delim = ", ";
        }
    } else if (std::is_same<T, el_point_t>::value) {
        ss = "\"points\": [";
        for (const auto& point : results) {
            ss += concat_strings(delim,
                                 "[",
                                 std::to_string(point.x),
                                 ", ",
                                 std::to_string(point.y),
                                 ", ",
                                 std::to_string(point.score),
                                 ", ",
                                 std::to_string(point.target),
                                 "]");
            delim = ", ";
        }
    } else if (std::is_same<T, el_class_t>::value) {
        ss = "\"classes\": [";
        for (const auto& cls : results) {
            ss += concat_strings(delim, "[", std::to_string(cls.score), ", ", std::to_string(cls.target), "]");
            delim = ", ";
        }
    }
    ss += "]";

    return ss;
}


std::string  box_results_2_json_str(std::forward_list<el_box_t>& results) {
    std::string ss;
    const char* delim = "";

    
    ss = "\"boxes\": [";
    for (const auto& box : results) {
        ss += concat_strings(delim,
                                "[",
                                std::to_string(box.x),
                                ", ",
                                std::to_string(box.y),
                                ", ",
                                std::to_string(box.w),
                                ", ",
                                std::to_string(box.h),
                                ", ",
                                std::to_string(box.score),
                                ", ",
                                std::to_string(box.target),
                                "]");
        delim = ", ";
    }
    ss += "]";

    return ss;
}


std::string  fm_face_bbox_results_2_json_str(std::forward_list<el_box_t>& results) {
    std::string ss;
    const char* delim = "";

    ss = "\"fm_face_boxes\": [";
    for (const auto& box : results) {
        ss += concat_strings(delim,
                                "[",
                                std::to_string(box.x),
                                ", ",
                                std::to_string(box.y),
                                ", ",
                                std::to_string(box.w),
                                ", ",
                                std::to_string(box.h),
                                ", ",
                                std::to_string(box.score),
                                ", ",
                                std::to_string(box.target),
                                "]");
        delim = ", ";
    }
    ss += "]";

    return ss;
}


std::string  fm_point_results_2_json_str(std::forward_list<el_fm_point_t>& results) {
    std::string ss;
    const char* delim = "";

    
    
    ss = "\"fm_points\": [";
        std::forward_list<el_fm_point_t>::iterator it;
        for ( it = results.begin(); it != results.end(); ++it )
        {
            ss += concat_strings(delim,
                                 "[[",
                                 std::to_string(it->el_box.x),
                                 ", ",
                                 std::to_string(it->el_box.y),
                                 ", ",
                                 std::to_string(it->el_box.w),
                                 ", ",
                                 std::to_string(it->el_box.h),
                                 ", ",
                                 std::to_string(it->el_box.score),
                                 ", ",
                                 std::to_string(it->el_box.target),
                                 "]");
            delim = ", [";

            for(int i = 0 ; i < FM_POINT_NUM ; i++)
            {
                ss += concat_strings(delim,
                                 "[",
                                 std::to_string(it->el_fm_point[i].x),
                                 ", ",
                                 std::to_string(it->el_fm_point[i].y),
                                 ", ",
                                 std::to_string(it->el_fm_point[i].score),
                                 ", ",
                                 std::to_string(it->el_fm_point[i].target),
                                 "]");
                delim = ", ";
            }

            delim = "] , [";
            for(int i = 0 ; i < FM_IRIS_POINT_NUM ; i++)
            {
                ss += concat_strings(delim,
                                 "[",
                                 std::to_string(it->el_fm_iris[i].x),
                                 ", ",
                                 std::to_string(it->el_fm_iris[i].y),
                                 ", ",
                                 std::to_string(it->el_fm_iris[i].score),
                                 ", ",
                                 std::to_string(it->el_fm_iris[i].target),
                                 "]");
                delim = ", ";
            }

            /////////////////////////////////////////
            ////yaw pitch roll
            delim = "] , [";

            ss += concat_strings(delim,
                                 "[",
                                 std::to_string(it->el_fm_angle.yaw),
                                 ", ",
                                 std::to_string(it->el_fm_angle.pitch),
                                 ", ",
                                 std::to_string(it->el_fm_angle.roll),
                                 ", ",
                                 std::to_string(it->el_fm_angle.MAR),
                                  ", ",
                                 std::to_string(it->el_fm_angle.LEAR),
                                  ", ",
                                 std::to_string(it->el_fm_angle.REAR),
                                  ", ",
                                 std::to_string(it->el_fm_angle.left_iris_theta),
                                  ", ",
                                 std::to_string(it->el_fm_angle.left_iris_phi),
                                  ", ",
                                 std::to_string(it->el_fm_angle.right_iris_theta),
                                  ", ",
                                 std::to_string(it->el_fm_angle.right_iris_phi),
                                 "]");

            /////////////////////////////////////////
            ss += "]]";
            delim = ", ";
        }
    ss += "]";
    return ss;
}


std::string  fd_fl_results_2_json_str(std::forward_list<el_fd_fl_t>& results)
{
    std::string ss;
    const char* delim = "";

    
    
    ss = "\"fd_fl\": [";
        std::forward_list<el_fd_fl_t>::iterator it;
        for ( it = results.begin(); it != results.end(); ++it )
        {
            ss += concat_strings(delim,
                                 "[[",
                                 std::to_string(it->el_box.x),
                                 ", ",
                                 std::to_string(it->el_box.y),
                                 ", ",
                                 std::to_string(it->el_box.w),
                                 ", ",
                                 std::to_string(it->el_box.h),
                                 ", ",
                                 std::to_string(it->el_box.score),
                                 ", ",
                                 std::to_string(it->el_box.target),
                                 "]");
            delim = ", [";

            for(int i = 0 ; i < MAX_FACE_LAND_MARK_TRACKED_POINT ; i++)
            {
                ss += concat_strings(delim,
                                 "[",
                                 std::to_string(it->el_fl[i].x),
                                 ", ",
                                 std::to_string(it->el_fl[i].y),
                                 ", ",
                                 std::to_string(it->el_fl[i].target),
                                 "]");
                delim = ", ";
            }

            /////////////////////////////////////////
            ////yaw pitch roll
            delim = "] , [";

            ss += concat_strings(delim,
                                 "[",
                                 std::to_string(it->el_fl_angle.yaw),
                                 ", ",
                                 std::to_string(it->el_fl_angle.pitch),
                                 ", ",
                                 std::to_string(it->el_fl_angle.roll),
                                 ", ",
                                 std::to_string(it->el_fl_angle.MAR),
                                  ", ",
                                 std::to_string(it->el_fl_angle.LEAR),
                                  ", ",
                                 std::to_string(it->el_fl_angle.REAR),
                                 "]");

            /////////////////////////////////////////
            ss += "]]";
            delim = ", ";
        }
    ss += "]";
    return ss;

}



std::string  fd_fl_el_9t_results_2_json_str(std::forward_list<el_fd_fl_el_9pt_t>& results)
{
    std::string ss;
    const char* delim = "";

    
    
    ss = "\"fd_fl_el_9pt\": [";
        std::forward_list<el_fd_fl_el_9pt_t>::iterator it;
        for ( it = results.begin(); it != results.end(); ++it )
        {
            ss += concat_strings(delim,
                                 "[[",
                                 std::to_string(it->el_box.x),
                                 ", ",
                                 std::to_string(it->el_box.y),
                                 ", ",
                                 std::to_string(it->el_box.w),
                                 ", ",
                                 std::to_string(it->el_box.h),
                                 ", ",
                                 std::to_string(it->el_box.score),
                                 ", ",
                                 std::to_string(it->el_box.target),
                                 "]");
            delim = ", [";

            for(int i = 0 ; i < MAX_FACE_LAND_MARK_TRACKED_POINT ; i++)
            {
                ss += concat_strings(delim,
                                 "[",
                                 std::to_string(it->el_fl[i].x),
                                 ", ",
                                 std::to_string(it->el_fl[i].y),
                                 ", ",
                                 std::to_string(it->el_fl[i].target),
                                 "]");
                delim = ", ";
            }

            ////////////////////////////////left eye landmark
            delim = "] , [";
            for(int i = 0 ; i < 9 ; i++)
            {
                ss += concat_strings(delim,
                                 "[",
                                 std::to_string(it->left_eye_landmark[i].x),
                                 ", ",
                                 std::to_string(it->left_eye_landmark[i].y),
                                 ", ",
                                 std::to_string(it->left_eye_landmark[i].target),
                                 "]");
                delim = ", ";
            }

            ////////////////////////////////right eye landmark

            delim = "] , [";
            for(int i = 0 ; i < 9 ; i++)
            {
                ss += concat_strings(delim,
                                 "[",
                                 std::to_string(it->right_eye_landmark[i].x),
                                 ", ",
                                 std::to_string(it->right_eye_landmark[i].y),
                                 ", ",
                                 std::to_string(it->right_eye_landmark[i].target),
                                 "]");
                delim = ", ";
            }

            /////////////////////////////////////////
            ////yaw pitch roll
            delim = "] , [";

            ss += concat_strings(delim,
                                 "[",
                                 std::to_string(it->el_fl_angle.yaw),
                                 ", ",
                                 std::to_string(it->el_fl_angle.pitch),
                                 ", ",
                                 std::to_string(it->el_fl_angle.roll),
                                 ", ",
                                 std::to_string(it->el_fl_angle.MAR),
                                  ", ",
                                 std::to_string(it->el_fl_angle.LEAR),
                                  ", ",
                                 std::to_string(it->el_fl_angle.REAR),
                                  ", ",
                                 std::to_string(it->el_fl_angle.left_iris_theta),
                                  ", ",
                                 std::to_string(it->el_fl_angle.left_iris_phi),
                                  ", ",
                                 std::to_string(it->el_fl_angle.right_iris_theta),
                                  ", ",
                                 std::to_string(it->el_fl_angle.right_iris_phi),
                                 "]");

            /////////////////////////////////////////
            ss += "]]";
            delim = ", ";
        }
    ss += "]";
    return ss;

}




std::string  keypoint_results_2_json_str(std::forward_list<el_keypoint_t>& results) {
    std::string ss;
    const char* delim = "";

    
    
    ss = "\"keypoints\": [";
        std::forward_list<el_keypoint_t>::iterator it;
        for ( it = results.begin(); it != results.end(); ++it )
        {
            ss += concat_strings(delim,
                                 "[[",
                                 std::to_string(it->el_box.x),
                                 ", ",
                                 std::to_string(it->el_box.y),
                                 ", ",
                                 std::to_string(it->el_box.w),
                                 ", ",
                                 std::to_string(it->el_box.h),
                                 ", ",
                                 std::to_string(it->el_box.score),
                                 ", ",
                                 std::to_string(it->el_box.target),
                                 "]");
            delim = ", [";
            for(int i = 0 ; i < KEYPOINT_NUM ; i++)
            {
                ss += concat_strings(delim,
                                 "[",
                                 std::to_string(it->el_keypoint[i].x),
                                 ", ",
                                 std::to_string(it->el_keypoint[i].y),
                                 ", ",
                                 std::to_string(it->el_keypoint[i].score),
                                 ", ",
                                 std::to_string(it->el_keypoint[i].target),
                                 "]");
                delim = ", ";
            }
            ss += "]]";
            delim = ", ";
        }
    ss += "]";

    return ss;
}


void event_reply(std::string data) {
std::string _cmd ;
std::size_t _n_times;
bool        _results_only;

std::size_t         _task_id;
// el_sensor_info_t    _sensor_info;
// el_model_info_t     _model_info;
// el_algorithm_info_t _algorithm_info;

std::size_t   _times=0;
el_err_code_t _ret;
uint16_t      _action_hash;
    const auto& ss{concat_strings("\r{\"type\": 1, \"name\": \"",
                                    "INVOKE",
                                    "\", \"code\": ",
                                    std::to_string(EL_OK),
                                    ", \"data\": {\"count\": ",
                                    std::to_string(_times),
                                    data,
                                    "}}\n")};
    send_bytes(ss.c_str(), ss.size());
}
inline std::string quoted(const std::string& str, const char delim = '"') {
    std::size_t sz = 0;
    for (char c : str)
        if (c == delim) [[unlikely]]
            ++sz;
    std::string ss(1, delim);
    ss.reserve(str.length() + (sz << 1));
    for (char c : str) {
        if (c == delim) [[unlikely]]
            ss += '\\';
        if (c != '\n') [[likely]]
            ss += c;
    }
    ss += delim;
    return ss;
}
const static uint16_t CRC16_MAXIM_TABLE[256] = {
  0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241, 0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1,
  0xc481, 0x0440, 0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40, 0x0a00, 0xcac1, 0xcb81, 0x0b40,
  0xc901, 0x09c0, 0x0880, 0xc841, 0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40, 0x1e00, 0xdec1,
  0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41, 0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641,
  0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040, 0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1,
  0xf281, 0x3240, 0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441, 0x3c00, 0xfcc1, 0xfd81, 0x3d40,
  0xff01, 0x3fc0, 0x3e80, 0xfe41, 0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840, 0x2800, 0xe8c1,
  0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41, 0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40,
  0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640, 0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0,
  0x2080, 0xe041, 0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240, 0x6600, 0xa6c1, 0xa781, 0x6740,
  0xa501, 0x65c0, 0x6480, 0xa441, 0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41, 0xaa01, 0x6ac0,
  0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840, 0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
  0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40, 0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1,
  0xb681, 0x7640, 0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041, 0x5000, 0x90c1, 0x9181, 0x5140,
  0x9301, 0x53c0, 0x5280, 0x9241, 0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440, 0x9c01, 0x5cc0,
  0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40, 0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841,
  0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40, 0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0,
  0x4c80, 0x8c41, 0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641, 0x8201, 0x42c0, 0x4380, 0x8341,
  0x4100, 0x81c1, 0x8081, 0x4040};

EL_ATTR_WEAK uint16_t el_crc16_maxim(const uint8_t* data, size_t length) {
    uint16_t crc = 0x0000;

    for (size_t i = 0; i < length; ++i) {
        uint8_t index = static_cast<uint8_t>(crc ^ data[i]);
        crc           = (crc >> 8) ^ CRC16_MAXIM_TABLE[index];
    }

    return crc ^ 0xffff;
}


std::string model_info_2_json_str(el_model_info_t model_info) {
    return concat_strings("{\"id\": ",
                          std::to_string(model_info.id),
                          ", \"type\": ",
                          std::to_string(model_info.type),
                          ", \"address\": ",
                          std::to_string(model_info.addr_flash),
                          ", \"size\": ",
                          std::to_string(model_info.size),
                          "}");
}
void send_device_id() {
    std::string cmd;

    const auto& ss{concat_strings("\r{\"type\": 0, \"name\": \"",
                                  "NAME?",
                                  "\", \"code\": ",
                                  std::to_string(EL_OK),
                                  ", \"data\": ",
                                  quoted("kris Grove Vision AI (WE2)"),
                                  "}\n")};
    send_bytes(ss.c_str(), ss.size());

    const auto& ss_ver{concat_strings("\r{\"type\": 0, \"name\": \"",
                                  "VER?",
                                  "\", \"code\": ",
                                  std::to_string(EL_OK),
                                  ", \"data\": {\"software\": \"",
                                  EL_VERSION,
                                  "\", \"hardware\": \"",
                                  "kris 2024",
                                  "\"}}\n")};
    send_bytes(ss_ver.c_str(), ss_ver.size());



    const auto& ss_id{concat_strings("\r{\"type\": 0, \"name\": \"",
                                  "ID?",
                                  "\", \"code\": ",
                                  std::to_string(EL_OK),
                                  ", \"data\": ",
                                  "1",
                                  "}\n")};
    send_bytes(ss_id.c_str(), ss_id.size());

     char info[CONFIG_SSCMA_CMD_MAX_LENGTH]{};

     const auto& ss_info{
      concat_strings("\r{\"type\": 0, \"name\": \"",
                     "INFO?",
                     "\", \"code\": ",
                     std::to_string(EL_OK),
                     ", \"data\": {\"crc16_maxim\": ",
                     std::to_string(el_crc16_maxim(reinterpret_cast<uint8_t*>(&info[0]), std::strlen(&info[0]))),
                     ", \"info\": ",
                     quoted(info),
                     "}}\n")};
    send_bytes(ss_info.c_str(), ss_info.size());
    el_model_info_t model_info;
    model_info.id=0;
    model_info.type=EL_ALGO_TYPE_UNDEFINED;

    const auto& ss_model_info{concat_strings("\r{\"type\": 0, \"name\": \"",
                                  "MODEL?",
                                  "\", \"code\": ",
                                  std::to_string(EL_OK),
                                  ", \"data\": ",
                                  model_info_2_json_str(model_info),
                                  "}\n")};
    send_bytes(ss_model_info.c_str(), ss_model_info.size());

}

std::string  algo_tick_2_json_str(uint32_t algo_tick) {
    std::string ss;
    const char* delim = "";

    ss = "\"algo_tick\": [";
    
    ss += concat_strings(delim,
                            "[",
                            std::to_string(algo_tick),
                            "]");
    delim = ", ";
    ss += "]";

    return ss;
}