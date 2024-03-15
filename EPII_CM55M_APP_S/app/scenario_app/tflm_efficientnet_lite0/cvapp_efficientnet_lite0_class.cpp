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
#include "cvapp_efficientnet_lite0_class.h"
#include "cisdp_sensor.h"

#include "WE2_core.h"

#include "ethosu_driver.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"

#include "img_proc_helium.h"


#include "xprintf.h"
#include "spi_master_protocol.h"
#include "cisdp_cfg.h"
#include "memory_manage.h"


#define INPUT_IMAGE_CHANNELS 3
#define EFF_LITE0_CLS_INPUT_TENSOR_WIDTH   224
#define EFF_LITE0_CLS_INPUT_TENSOR_HEIGHT  224
#define EFF_LITE0_CLS_INPUT_TENSOR_CHANNEL INPUT_IMAGE_CHANNELS

#define  INVOKE_STEP_TICK 0

#ifdef TRUSTZONE_SEC
#define U55_BASE	BASE_ADDR_APB_U55_CTRL_ALIAS
#else
#ifndef TRUSTZONE
#define U55_BASE	BASE_ADDR_APB_U55_CTRL_ALIAS
#else
#define U55_BASE	BASE_ADDR_APB_U55_CTRL
#endif
#endif


using namespace std;

namespace {


// constexpr int tensor_arena_size = 375*1024;
constexpr int tensor_arena_size = 354*1024;


static uint32_t tensor_arena=0;

struct ethosu_driver ethosu_drv; /* Default Ethos-U device driver */
tflite::MicroInterpreter *eff_lite0_cls_int_ptr=nullptr;
TfLiteTensor *eff_lite0_cls_input, *eff_lite0_cls_output;

};


#define CPU_CLK	0xffffff+1
uint32_t systick_1, systick_2;
uint32_t loop_cnt_1, loop_cnt_2;


std::string imagenet_class_label[1000];


// #endif

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
    const void * ethosu_base_address = (void *)(U55_BASE);

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

int cv_efficientnet_lite0_class_init(bool security_enable, bool privilege_enable, uint32_t model_addr) {
	
      imagenet_class_label[0]= "tench, Tinca tinca";
      imagenet_class_label[1]= "goldfish, Carassius auratus";
      imagenet_class_label[2]= "great white shark, white shark, man-eater, man-eating shark, Carcharodon carcharias";
      imagenet_class_label[3]= "tiger shark, Galeocerdo cuvieri";
      imagenet_class_label[4]= "hammerhead, hammerhead shark";
      imagenet_class_label[5]= "electric ray, crampfish, numbfish, torpedo";
      imagenet_class_label[6]= "stingray";
      imagenet_class_label[7]= "cock";
      imagenet_class_label[8]= "hen";
      imagenet_class_label[9]= "ostrich, Struthio camelus";
      imagenet_class_label[10]= "brambling, Fringilla montifringilla";
      imagenet_class_label[11]= "goldfinch, Carduelis carduelis";
      imagenet_class_label[12]= "house finch, linnet, Carpodacus mexicanus";
      imagenet_class_label[13]= "junco, snowbird";
      imagenet_class_label[14]= "indigo bunting, indigo finch, indigo bird, Passerina cyanea";
      imagenet_class_label[15]= "robin, American robin, Turdus migratorius";
      imagenet_class_label[16]= "bulbul";
      imagenet_class_label[17]= "jay";
      imagenet_class_label[18]= "magpie";
      imagenet_class_label[19]= "chickadee";
      imagenet_class_label[20]= "water ouzel, dipper";
      imagenet_class_label[21]= "kite";
      imagenet_class_label[22]= "bald eagle, American eagle, Haliaeetus leucocephalus";
      imagenet_class_label[23]= "vulture";
      imagenet_class_label[24]= "great grey owl, great gray owl, Strix nebulosa";
      imagenet_class_label[25]= "European fire salamander, Salamandra salamandra";
      imagenet_class_label[26]= "common newt, Triturus vulgaris";
      imagenet_class_label[27]= "eft";
      imagenet_class_label[28]= "spotted salamander, Ambystoma maculatum";
      imagenet_class_label[29]= "axolotl, mud puppy, Ambystoma mexicanum";
      imagenet_class_label[30]= "bullfrog, Rana catesbeiana";
      imagenet_class_label[31]= "tree frog, tree-frog";
      imagenet_class_label[32]= "tailed frog, bell toad, ribbed toad, tailed toad, Ascaphus trui";
      imagenet_class_label[33]= "loggerhead, loggerhead turtle, Caretta caretta";
      imagenet_class_label[34]= "leatherback turtle, leatherback, leathery turtle, Dermochelys coriacea";
      imagenet_class_label[35]= "mud turtle";
      imagenet_class_label[36]= "terrapin";
      imagenet_class_label[37]= "box turtle, box tortoise";
      imagenet_class_label[38]= "banded gecko";
      imagenet_class_label[39]= "common iguana, iguana, Iguana iguana";
      imagenet_class_label[40]= "American chameleon, anole, Anolis carolinensis";
      imagenet_class_label[41]= "whiptail, whiptail lizard";
      imagenet_class_label[42]= "agama";
      imagenet_class_label[43]= "frilled lizard, Chlamydosaurus kingi";
      imagenet_class_label[44]= "alligator lizard";
      imagenet_class_label[45]= "Gila monster, Heloderma suspectum";
      imagenet_class_label[46]= "green lizard, Lacerta viridis";
      imagenet_class_label[47]= "African chameleon, Chamaeleo chamaeleon";
      imagenet_class_label[48]= "Komodo dragon, Komodo lizard, dragon lizard, giant lizard, Varanus komodoensis";
      imagenet_class_label[49]= "African crocodile, Nile crocodile, Crocodylus niloticus";
      imagenet_class_label[50]= "American alligator, Alligator mississipiensis";
      imagenet_class_label[51]= "triceratops";
      imagenet_class_label[52]= "thunder snake, worm snake, Carphophis amoenus";
      imagenet_class_label[53]= "ringneck snake, ring-necked snake, ring snake";
      imagenet_class_label[54]= "hognose snake, puff adder, sand viper";
      imagenet_class_label[55]= "green snake, grass snake";
      imagenet_class_label[56]= "king snake, kingsnake";
      imagenet_class_label[57]= "garter snake, grass snake";
      imagenet_class_label[58]= "water snake";
      imagenet_class_label[59]= "vine snake";
      imagenet_class_label[60]= "night snake, Hypsiglena torquata";
      imagenet_class_label[61]= "boa constrictor, Constrictor constrictor";
      imagenet_class_label[62]= "rock python, rock snake, Python sebae";
      imagenet_class_label[63]= "Indian cobra, Naja naja";
      imagenet_class_label[64]= "green mamba";
      imagenet_class_label[65]= "sea snake";
      imagenet_class_label[66]= "horned viper, cerastes, sand viper, horned asp, Cerastes cornutus";
      imagenet_class_label[67]= "diamondback, diamondback rattlesnake, Crotalus adamanteus";
      imagenet_class_label[68]= "sidewinder, horned rattlesnake, Crotalus cerastes";
      imagenet_class_label[69]= "trilobite";
      imagenet_class_label[70]= "harvestman, daddy longlegs, Phalangium opilio";
      imagenet_class_label[71]= "scorpion";
      imagenet_class_label[72]= "black and gold garden spider, Argiope aurantia";
      imagenet_class_label[73]= "barn spider, Araneus cavaticus";
      imagenet_class_label[74]= "garden spider, Aranea diademata";
      imagenet_class_label[75]= "black widow, Latrodectus mactans";
      imagenet_class_label[76]= "tarantula";
      imagenet_class_label[77]= "wolf spider, hunting spider";
      imagenet_class_label[78]= "tick";
      imagenet_class_label[79]= "centipede";
      imagenet_class_label[80]= "black grouse";
      imagenet_class_label[81]= "ptarmigan";
      imagenet_class_label[82]= "ruffed grouse, partridge, Bonasa umbellus";
      imagenet_class_label[83]= "prairie chicken, prairie grouse, prairie fowl";
      imagenet_class_label[84]= "peacock";
      imagenet_class_label[85]= "quail";
      imagenet_class_label[86]= "partridge";
      imagenet_class_label[87]= "African grey, African gray, Psittacus erithacus";
      imagenet_class_label[88]= "macaw";
      imagenet_class_label[89]= "sulphur-crested cockatoo, Kakatoe galerita, Cacatua galerita";
      imagenet_class_label[90]= "lorikeet";
      imagenet_class_label[91]= "coucal";
      imagenet_class_label[92]= "bee eater";
      imagenet_class_label[93]= "hornbill";
      imagenet_class_label[94]= "hummingbird";
      imagenet_class_label[95]= "jacamar";
      imagenet_class_label[96]= "toucan";
      imagenet_class_label[97]= "drake";
      imagenet_class_label[98]= "red-breasted merganser, Mergus serrator";
      imagenet_class_label[99]= "goose";
      imagenet_class_label[100]= "black swan, Cygnus atratus";
      imagenet_class_label[101]= "tusker";
      imagenet_class_label[102]= "echidna, spiny anteater, anteater";
      imagenet_class_label[103]= "platypus, duckbill, duckbilled platypus, duck-billed platypus, Ornithorhynchus anatinus";
      imagenet_class_label[104]= "wallaby, brush kangaroo";
      imagenet_class_label[105]= "koala, koala bear, kangaroo bear, native bear, Phascolarctos cinereus";
      imagenet_class_label[106]= "wombat";
      imagenet_class_label[107]= "jellyfish";
      imagenet_class_label[108]= "sea anemone, anemone";
      imagenet_class_label[109]= "brain coral";
      imagenet_class_label[110]= "flatworm, platyhelminth";
      imagenet_class_label[111]= "nematode, nematode worm, roundworm";
      imagenet_class_label[112]= "conch";
      imagenet_class_label[113]= "snail";
      imagenet_class_label[114]= "slug";
      imagenet_class_label[115]= "sea slug, nudibranch";
      imagenet_class_label[116]= "chiton, coat-of-mail shell, sea cradle, polyplacophore";
      imagenet_class_label[117]= "chambered nautilus, pearly nautilus, nautilus";
      imagenet_class_label[118]= "Dungeness crab, Cancer magister";
      imagenet_class_label[119]= "rock crab, Cancer irroratus";
      imagenet_class_label[120]= "fiddler crab";
      imagenet_class_label[121]= "king crab, Alaska crab, Alaskan king crab, Alaska king crab, Paralithodes camtschatica";
      imagenet_class_label[122]= "American lobster, Northern lobster, Maine lobster, Homarus americanus";
      imagenet_class_label[123]= "spiny lobster, langouste, rock lobster, crawfish, crayfish, sea crawfish";
      imagenet_class_label[124]= "crayfish, crawfish, crawdad, crawdaddy";
      imagenet_class_label[125]= "hermit crab";
      imagenet_class_label[126]= "isopod";
      imagenet_class_label[127]= "white stork, Ciconia ciconia";
      imagenet_class_label[128]= "black stork, Ciconia nigra";
      imagenet_class_label[129]= "spoonbill";
      imagenet_class_label[130]= "flamingo";
      imagenet_class_label[131]= "little blue heron, Egretta caerulea";
      imagenet_class_label[132]= "American egret, great white heron, Egretta albus";
      imagenet_class_label[133]= "bittern";
      imagenet_class_label[134]= "crane";
      imagenet_class_label[135]= "limpkin, Aramus pictus";
      imagenet_class_label[136]= "European gallinule, Porphyrio porphyrio";
      imagenet_class_label[137]= "American coot, marsh hen, mud hen, water hen, Fulica americana";
      imagenet_class_label[138]= "bustard";
      imagenet_class_label[139]= "ruddy turnstone, Arenaria interpres";
      imagenet_class_label[140]= "red-backed sandpiper, dunlin, Erolia alpina";
      imagenet_class_label[141]= "redshank, Tringa totanus";
      imagenet_class_label[142]= "dowitcher";
      imagenet_class_label[143]= "oystercatcher, oyster catcher";
      imagenet_class_label[144]= "pelican";
      imagenet_class_label[145]= "king penguin, Aptenodytes patagonica";
      imagenet_class_label[146]= "albatross, mollymawk";
      imagenet_class_label[147]= "grey whale, gray whale, devilfish, Eschrichtius gibbosus, Eschrichtius robustus";
      imagenet_class_label[148]= "killer whale, killer, orca, grampus, sea wolf, Orcinus orca";
      imagenet_class_label[149]= "dugong, Dugong dugon";
      imagenet_class_label[150]= "sea lion";
      imagenet_class_label[151]= "Chihuahua";
      imagenet_class_label[152]= "Japanese spaniel";
      imagenet_class_label[153]= "Maltese dog, Maltese terrier, Maltese";
      imagenet_class_label[154]= "Pekinese, Pekingese, Peke";
      imagenet_class_label[155]= "Shih-Tzu";
      imagenet_class_label[156]= "Blenheim spaniel";
      imagenet_class_label[157]= "papillon";
      imagenet_class_label[158]= "toy terrier";
      imagenet_class_label[159]= "Rhodesian ridgeback";
      imagenet_class_label[160]= "Afghan hound, Afghan";
      imagenet_class_label[161]= "basset, basset hound";
      imagenet_class_label[162]= "beagle";
      imagenet_class_label[163]= "bloodhound, sleuthhound";
      imagenet_class_label[164]= "bluetick";
      imagenet_class_label[165]= "black-and-tan coonhound";
      imagenet_class_label[166]= "Walker hound, Walker foxhound";
      imagenet_class_label[167]= "English foxhound";
      imagenet_class_label[168]= "redbone";
      imagenet_class_label[169]= "borzoi, Russian wolfhound";
      imagenet_class_label[170]= "Irish wolfhound";
      imagenet_class_label[171]= "Italian greyhound";
      imagenet_class_label[172]= "whippet";
      imagenet_class_label[173]= "Ibizan hound, Ibizan Podenco";
      imagenet_class_label[174]= "Norwegian elkhound, elkhound";
      imagenet_class_label[175]= "otterhound, otter hound";
      imagenet_class_label[176]= "Saluki, gazelle hound";
      imagenet_class_label[177]= "Scottish deerhound, deerhound";
      imagenet_class_label[178]= "Weimaraner";
      imagenet_class_label[179]= "Staffordshire bullterrier, Staffordshire bull terrier";
      imagenet_class_label[180]= "American Staffordshire terrier, Staffordshire terrier, American pit bull terrier, pit bull terrier";
      imagenet_class_label[181]= "Bedlington terrier";
      imagenet_class_label[182]= "Border terrier";
      imagenet_class_label[183]= "Kerry blue terrier";
      imagenet_class_label[184]= "Irish terrier";
      imagenet_class_label[185]= "Norfolk terrier";
      imagenet_class_label[186]= "Norwich terrier";
      imagenet_class_label[187]= "Yorkshire terrier";
      imagenet_class_label[188]= "wire-haired fox terrier";
      imagenet_class_label[189]= "Lakeland terrier";
      imagenet_class_label[190]= "Sealyham terrier, Sealyham";
      imagenet_class_label[191]= "Airedale, Airedale terrier";
      imagenet_class_label[192]= "cairn, cairn terrier";
      imagenet_class_label[193]= "Australian terrier";
      imagenet_class_label[194]= "Dandie Dinmont, Dandie Dinmont terrier";
      imagenet_class_label[195]= "Boston bull, Boston terrier";
      imagenet_class_label[196]= "miniature schnauzer";
      imagenet_class_label[197]= "giant schnauzer";
      imagenet_class_label[198]= "standard schnauzer";
      imagenet_class_label[199]= "Scotch terrier, Scottish terrier, Scottie";
      imagenet_class_label[200]= "Tibetan terrier, chrysanthemum dog";
      imagenet_class_label[201]= "silky terrier, Sydney silky";
      imagenet_class_label[202]= "soft-coated wheaten terrier";
      imagenet_class_label[203]= "West Highland white terrier";
      imagenet_class_label[204]= "Lhasa, Lhasa apso";
      imagenet_class_label[205]= "flat-coated retriever";
      imagenet_class_label[206]= "curly-coated retriever";
      imagenet_class_label[207]= "golden retriever";
      imagenet_class_label[208]= "Labrador retriever";
      imagenet_class_label[209]= "Chesapeake Bay retriever";
      imagenet_class_label[210]= "German short-haired pointer";
      imagenet_class_label[211]= "vizsla, Hungarian pointer";
      imagenet_class_label[212]= "English setter";
      imagenet_class_label[213]= "Irish setter, red setter";
      imagenet_class_label[214]= "Gordon setter";
      imagenet_class_label[215]= "Brittany spaniel";
      imagenet_class_label[216]= "clumber, clumber spaniel";
      imagenet_class_label[217]= "English springer, English springer spaniel";
      imagenet_class_label[218]= "Welsh springer spaniel";
      imagenet_class_label[219]= "cocker spaniel, English cocker spaniel, cocker";
      imagenet_class_label[220]= "Sussex spaniel";
      imagenet_class_label[221]= "Irish water spaniel";
      imagenet_class_label[222]= "kuvasz";
      imagenet_class_label[223]= "schipperke";
      imagenet_class_label[224]= "groenendael";
      imagenet_class_label[225]= "malinois";
      imagenet_class_label[226]= "briard";
      imagenet_class_label[227]= "kelpie";
      imagenet_class_label[228]= "komondor";
      imagenet_class_label[229]= "Old English sheepdog, bobtail";
      imagenet_class_label[230]= "Shetland sheepdog, Shetland sheep dog, Shetland";
      imagenet_class_label[231]= "collie";
      imagenet_class_label[232]= "Border collie";
      imagenet_class_label[233]= "Bouvier des Flandres, Bouviers des Flandres";
      imagenet_class_label[234]= "Rottweiler";
      imagenet_class_label[235]= "German shepherd, German shepherd dog, German police dog, alsatian";
      imagenet_class_label[236]= "Doberman, Doberman pinscher";
      imagenet_class_label[237]= "miniature pinscher";
      imagenet_class_label[238]= "Greater Swiss Mountain dog";
      imagenet_class_label[239]= "Bernese mountain dog";
      imagenet_class_label[240]= "Appenzeller";
      imagenet_class_label[241]= "EntleBucher";
      imagenet_class_label[242]= "boxer";
      imagenet_class_label[243]= "bull mastiff";
      imagenet_class_label[244]= "Tibetan mastiff";
      imagenet_class_label[245]= "French bulldog";
      imagenet_class_label[246]= "Great Dane";
      imagenet_class_label[247]= "Saint Bernard, St Bernard";
      imagenet_class_label[248]= "Eskimo dog, husky";
      imagenet_class_label[249]= "malamute, malemute, Alaskan malamute";
      imagenet_class_label[250]= "Siberian husky";
      imagenet_class_label[251]= "dalmatian, coach dog, carriage dog";
      imagenet_class_label[252]= "affenpinscher, monkey pinscher, monkey dog";
      imagenet_class_label[253]= "basenji";
      imagenet_class_label[254]= "pug, pug-dog";
      imagenet_class_label[255]= "Leonberg";
      imagenet_class_label[256]= "Newfoundland, Newfoundland dog";
      imagenet_class_label[257]= "Great Pyrenees";
      imagenet_class_label[258]= "Samoyed, Samoyede";
      imagenet_class_label[259]= "Pomeranian";
      imagenet_class_label[260]= "chow, chow chow";
      imagenet_class_label[261]= "keeshond";
      imagenet_class_label[262]= "Brabancon griffon";
      imagenet_class_label[263]= "Pembroke, Pembroke Welsh corgi";
      imagenet_class_label[264]= "Cardigan, Cardigan Welsh corgi";
      imagenet_class_label[265]= "toy poodle";
      imagenet_class_label[266]= "miniature poodle";
      imagenet_class_label[267]= "standard poodle";
      imagenet_class_label[268]= "Mexican hairless";
      imagenet_class_label[269]= "timber wolf, grey wolf, gray wolf, Canis lupus";
      imagenet_class_label[270]= "white wolf, Arctic wolf, Canis lupus tundrarum";
      imagenet_class_label[271]= "red wolf, maned wolf, Canis rufus, Canis niger";
      imagenet_class_label[272]= "coyote, prairie wolf, brush wolf, Canis latrans";
      imagenet_class_label[273]= "dingo, warrigal, warragal, Canis dingo";
      imagenet_class_label[274]= "dhole, Cuon alpinus";
      imagenet_class_label[275]= "African hunting dog, hyena dog, Cape hunting dog, Lycaon pictus";
      imagenet_class_label[276]= "hyena, hyaena";
      imagenet_class_label[277]= "red fox, Vulpes vulpes";
      imagenet_class_label[278]= "kit fox, Vulpes macrotis";
      imagenet_class_label[279]= "Arctic fox, white fox, Alopex lagopus";
      imagenet_class_label[280]= "grey fox, gray fox, Urocyon cinereoargenteus";
      imagenet_class_label[281]= "tabby, tabby cat";
      imagenet_class_label[282]= "tiger cat";
      imagenet_class_label[283]= "Persian cat";
      imagenet_class_label[284]= "Siamese cat, Siamese";
      imagenet_class_label[285]= "Egyptian cat";
      imagenet_class_label[286]= "cougar, puma, catamount, mountain lion, painter, panther, Felis concolor";
      imagenet_class_label[287]= "lynx, catamount";
      imagenet_class_label[288]= "leopard, Panthera pardus";
      imagenet_class_label[289]= "snow leopard, ounce, Panthera uncia";
      imagenet_class_label[290]= "jaguar, panther, Panthera onca, Felis onca";
      imagenet_class_label[291]= "lion, king of beasts, Panthera leo";
      imagenet_class_label[292]= "tiger, Panthera tigris";
      imagenet_class_label[293]= "cheetah, chetah, Acinonyx jubatus";
      imagenet_class_label[294]= "brown bear, bruin, Ursus arctos";
      imagenet_class_label[295]= "American black bear, black bear, Ursus americanus, Euarctos americanus";
      imagenet_class_label[296]= "ice bear, polar bear, Ursus Maritimus, Thalarctos maritimus";
      imagenet_class_label[297]= "sloth bear, Melursus ursinus, Ursus ursinus";
      imagenet_class_label[298]= "mongoose";
      imagenet_class_label[299]= "meerkat, mierkat";
      imagenet_class_label[300]= "tiger beetle";
      imagenet_class_label[301]= "ladybug, ladybeetle, lady beetle, ladybird, ladybird beetle";
      imagenet_class_label[302]= "ground beetle, carabid beetle";
      imagenet_class_label[303]= "long-horned beetle, longicorn, longicorn beetle";
      imagenet_class_label[304]= "leaf beetle, chrysomelid";
      imagenet_class_label[305]= "dung beetle";
      imagenet_class_label[306]= "rhinoceros beetle";
      imagenet_class_label[307]= "weevil";
      imagenet_class_label[308]= "fly";
      imagenet_class_label[309]= "bee";
      imagenet_class_label[310]= "ant, emmet, pismire";
      imagenet_class_label[311]= "grasshopper, hopper";
      imagenet_class_label[312]= "cricket";
      imagenet_class_label[313]= "walking stick, walkingstick, stick insect";
      imagenet_class_label[314]= "cockroach, roach";
      imagenet_class_label[315]= "mantis, mantid";
      imagenet_class_label[316]= "cicada, cicala";
      imagenet_class_label[317]= "leafhopper";
      imagenet_class_label[318]= "lacewing, lacewing fly";
      imagenet_class_label[319]= "dragonfly, darning needle, devils darning needle, sewing needle, snake feeder, snake doctor, mosquito hawk, skeeter hawk";
      imagenet_class_label[320]= "damselfly";
      imagenet_class_label[321]= "admiral";
      imagenet_class_label[322]= "ringlet, ringlet butterfly";
      imagenet_class_label[323]= "monarch, monarch butterfly, milkweed butterfly, Danaus plexippus";
      imagenet_class_label[324]= "cabbage butterfly";
      imagenet_class_label[325]= "sulphur butterfly, sulfur butterfly";
      imagenet_class_label[326]= "lycaenid, lycaenid butterfly";
      imagenet_class_label[327]= "starfish, sea star";
      imagenet_class_label[328]= "sea urchin";
      imagenet_class_label[329]= "sea cucumber, holothurian";
      imagenet_class_label[330]= "wood rabbit, cottontail, cottontail rabbit";
      imagenet_class_label[331]= "hare";
      imagenet_class_label[332]= "Angora, Angora rabbit";
      imagenet_class_label[333]= "hamster";
      imagenet_class_label[334]= "porcupine, hedgehog";
      imagenet_class_label[335]= "fox squirrel, eastern fox squirrel, Sciurus niger";
      imagenet_class_label[336]= "marmot";
      imagenet_class_label[337]= "beaver";
      imagenet_class_label[338]= "guinea pig, Cavia cobaya";
      imagenet_class_label[339]= "sorrel";
      imagenet_class_label[340]= "zebra";
      imagenet_class_label[341]= "hog, pig, grunter, squealer, Sus scrofa";
      imagenet_class_label[342]= "wild boar, boar, Sus scrofa";
      imagenet_class_label[343]= "warthog";
      imagenet_class_label[344]= "hippopotamus, hippo, river horse, Hippopotamus amphibius";
      imagenet_class_label[345]= "ox";
      imagenet_class_label[346]= "water buffalo, water ox, Asiatic buffalo, Bubalus bubalis";
      imagenet_class_label[347]= "bison";
      imagenet_class_label[348]= "ram, tup";
      imagenet_class_label[349]= "bighorn, bighorn sheep, cimarron, Rocky Mountain bighorn, Rocky Mountain sheep, Ovis canadensis";
      imagenet_class_label[350]= "ibex, Capra ibex";
      imagenet_class_label[351]= "hartebeest";
      imagenet_class_label[352]= "impala, Aepyceros melampus";
      imagenet_class_label[353]= "gazelle";
      imagenet_class_label[354]= "Arabian camel, dromedary, Camelus dromedarius";
      imagenet_class_label[355]= "llama";
      imagenet_class_label[356]= "weasel";
      imagenet_class_label[357]= "mink";
      imagenet_class_label[358]= "polecat, fitch, foulmart, foumart, Mustela putorius";
      imagenet_class_label[359]= "black-footed ferret, ferret, Mustela nigripes";
      imagenet_class_label[360]= "otter";
      imagenet_class_label[361]= "skunk, polecat, wood pussy";
      imagenet_class_label[362]= "badger";
      imagenet_class_label[363]= "armadillo";
      imagenet_class_label[364]= "three-toed sloth, ai, Bradypus tridactylus";
      imagenet_class_label[365]= "orangutan, orang, orangutang, Pongo pygmaeus";
      imagenet_class_label[366]= "gorilla, Gorilla gorilla";
      imagenet_class_label[367]= "chimpanzee, chimp, Pan troglodytes";
      imagenet_class_label[368]= "gibbon, Hylobates lar";
      imagenet_class_label[369]= "siamang, Hylobates syndactylus, Symphalangus syndactylus";
      imagenet_class_label[370]= "guenon, guenon monkey";
      imagenet_class_label[371]= "patas, hussar monkey, Erythrocebus patas";
      imagenet_class_label[372]= "baboon";
      imagenet_class_label[373]= "macaque";
      imagenet_class_label[374]= "langur";
      imagenet_class_label[375]= "colobus, colobus monkey";
      imagenet_class_label[376]= "proboscis monkey, Nasalis larvatus";
      imagenet_class_label[377]= "marmoset";
      imagenet_class_label[378]= "capuchin, ringtail, Cebus capucinus";
      imagenet_class_label[379]= "howler monkey, howler";
      imagenet_class_label[380]= "titi, titi monkey";
      imagenet_class_label[381]= "spider monkey, Ateles geoffroyi";
      imagenet_class_label[382]= "squirrel monkey, Saimiri sciureus";
      imagenet_class_label[383]= "Madagascar cat, ring-tailed lemur, Lemur catta";
      imagenet_class_label[384]= "indri, indris, Indri indri, Indri brevicaudatus";
      imagenet_class_label[385]= "Indian elephant, Elephas maximus";
      imagenet_class_label[386]= "African elephant, Loxodonta africana";
      imagenet_class_label[387]= "lesser panda, red panda, panda, bear cat, cat bear, Ailurus fulgens";
      imagenet_class_label[388]= "giant panda, panda, panda bear, coon bear, Ailuropoda melanoleuca";
      imagenet_class_label[389]= "barracouta, snoek";
      imagenet_class_label[390]= "eel";
      imagenet_class_label[391]= "coho, cohoe, coho salmon, blue jack, silver salmon, Oncorhynchus kisutch";
      imagenet_class_label[392]= "rock beauty, Holocanthus tricolor";
      imagenet_class_label[393]= "anemone fish";
      imagenet_class_label[394]= "sturgeon";
      imagenet_class_label[395]= "gar, garfish, garpike, billfish, Lepisosteus osseus";
      imagenet_class_label[396]= "lionfish";
      imagenet_class_label[397]= "puffer, pufferfish, blowfish, globefish";
      imagenet_class_label[398]= "abacus";
      imagenet_class_label[399]= "abaya";
      imagenet_class_label[400]= "academic gown, academic robe, judges robe";
      imagenet_class_label[401]= "accordion, piano accordion, squeeze box";
      imagenet_class_label[402]= "acoustic guitar";
      imagenet_class_label[403]= "aircraft carrier, carrier, flattop, attack aircraft carrier";
      imagenet_class_label[404]= "airliner";
      imagenet_class_label[405]= "airship, dirigible";
      imagenet_class_label[406]= "altar";
      imagenet_class_label[407]= "ambulance";
      imagenet_class_label[408]= "amphibian, amphibious vehicle";
      imagenet_class_label[409]= "analog clock";
      imagenet_class_label[410]= "apiary, bee house";
      imagenet_class_label[411]= "apron";
      imagenet_class_label[412]= "ashcan, trash can, garbage can, wastebin, ash bin, ash-bin, ashbin, dustbin, trash barrel, trash bin";
      imagenet_class_label[413]= "assault rifle, assault gun";
      imagenet_class_label[414]= "backpack, back pack, knapsack, packsack, rucksack, haversack";
      imagenet_class_label[415]= "bakery, bakeshop, bakehouse";
      imagenet_class_label[416]= "balance beam, beam";
      imagenet_class_label[417]= "balloon";
      imagenet_class_label[418]= "ballpoint, ballpoint pen, ballpen, Biro";
      imagenet_class_label[419]= "Band Aid";
      imagenet_class_label[420]= "banjo";
      imagenet_class_label[421]= "bannister, banister, balustrade, balusters, handrail";
      imagenet_class_label[422]= "barbell";
      imagenet_class_label[423]= "barber chair";
      imagenet_class_label[424]= "barbershop";
      imagenet_class_label[425]= "barn";
      imagenet_class_label[426]= "barometer";
      imagenet_class_label[427]= "barrel, cask";
      imagenet_class_label[428]= "barrow, garden cart, lawn cart, wheelbarrow";
      imagenet_class_label[429]= "baseball";
      imagenet_class_label[430]= "basketball";
      imagenet_class_label[431]= "bassinet";
      imagenet_class_label[432]= "bassoon";
      imagenet_class_label[433]= "bathing cap, swimming cap";
      imagenet_class_label[434]= "bath towel";
      imagenet_class_label[435]= "bathtub, bathing tub, bath, tub";
      imagenet_class_label[436]= "beach wagon, station wagon, wagon, estate car, beach waggon, station waggon, waggon";
      imagenet_class_label[437]= "beacon, lighthouse, beacon light, pharos";
      imagenet_class_label[438]= "beaker";
      imagenet_class_label[439]= "bearskin, busby, shako";
      imagenet_class_label[440]= "beer bottle";
      imagenet_class_label[441]= "beer glass";
      imagenet_class_label[442]= "bell cote, bell cot";
      imagenet_class_label[443]= "bib";
      imagenet_class_label[444]= "bicycle-built-for-two, tandem bicycle, tandem";
      imagenet_class_label[445]= "bikini, two-piece";
      imagenet_class_label[446]= "binder, ring-binder";
      imagenet_class_label[447]= "binoculars, field glasses, opera glasses";
      imagenet_class_label[448]= "birdhouse";
      imagenet_class_label[449]= "boathouse";
      imagenet_class_label[450]= "bobsled, bobsleigh, bob";
      imagenet_class_label[451]= "bolo tie, bolo, bola tie, bola";
      imagenet_class_label[452]= "bonnet, poke bonnet";
      imagenet_class_label[453]= "bookcase";
      imagenet_class_label[454]= "bookshop, bookstore, bookstall";
      imagenet_class_label[455]= "bottlecap";
      imagenet_class_label[456]= "bow";
      imagenet_class_label[457]= "bow tie, bow-tie, bowtie";
      imagenet_class_label[458]= "brass, memorial tablet, plaque";
      imagenet_class_label[459]= "brassiere, bra, bandeau";
      imagenet_class_label[460]= "breakwater, groin, groyne, mole, bulwark, seawall, jetty";
      imagenet_class_label[461]= "breastplate, aegis, egis";
      imagenet_class_label[462]= "broom";
      imagenet_class_label[463]= "bucket, pail";
      imagenet_class_label[464]= "buckle";
      imagenet_class_label[465]= "bulletproof vest";
      imagenet_class_label[466]= "bullet train, bullet";
      imagenet_class_label[467]= "butcher shop, meat market";
      imagenet_class_label[468]= "cab, hack, taxi, taxicab";
      imagenet_class_label[469]= "caldron, cauldron";
      imagenet_class_label[470]= "candle, taper, wax light";
      imagenet_class_label[471]= "cannon";
      imagenet_class_label[472]= "canoe";
      imagenet_class_label[473]= "can opener, tin opener";
      imagenet_class_label[474]= "cardigan";
      imagenet_class_label[475]= "car mirror";
      imagenet_class_label[476]= "carousel, carrousel, merry-go-round, roundabout, whirligig";
      imagenet_class_label[477]= "carpenters kit, tool kit";
      imagenet_class_label[478]= "carton";
      imagenet_class_label[479]= "car wheel";
      imagenet_class_label[480]= "cash machine, cash dispenser, automated teller machine, automatic teller machine, automated teller, automatic teller, ATM";
      imagenet_class_label[481]= "cassette";
      imagenet_class_label[482]= "cassette player";
      imagenet_class_label[483]= "castle";
      imagenet_class_label[484]= "catamaran";
      imagenet_class_label[485]= "CD player";
      imagenet_class_label[486]= "cello, violoncello";
      imagenet_class_label[487]= "cellular telephone, cellular phone, cellphone, cell, mobile phone";
      imagenet_class_label[488]= "chain";
      imagenet_class_label[489]= "chainlink fence";
      imagenet_class_label[490]= "chain mail, ring mail, mail, chain armor, chain armour, ring armor, ring armour";
      imagenet_class_label[491]= "chain saw, chainsaw";
      imagenet_class_label[492]= "chest";
      imagenet_class_label[493]= "chiffonier, commode";
      imagenet_class_label[494]= "chime, bell, gong";
      imagenet_class_label[495]= "china cabinet, china closet";
      imagenet_class_label[496]= "Christmas stocking";
      imagenet_class_label[497]= "church, church building";
      imagenet_class_label[498]= "cinema, movie theater, movie theatre, movie house, picture palace";
      imagenet_class_label[499]= "cleaver, meat cleaver, chopper";
      imagenet_class_label[500]= "cliff dwelling";
      imagenet_class_label[501]= "cloak";
      imagenet_class_label[502]= "clog, geta, patten, sabot";
      imagenet_class_label[503]= "cocktail shaker";
      imagenet_class_label[504]= "coffee mug";
      imagenet_class_label[505]= "coffeepot";
      imagenet_class_label[506]= "coil, spiral, volute, whorl, helix";
      imagenet_class_label[507]= "combination lock";
      imagenet_class_label[508]= "computer keyboard, keypad";
      imagenet_class_label[509]= "confectionery, confectionary, candy store";
      imagenet_class_label[510]= "container ship, containership, container vessel";
      imagenet_class_label[511]= "convertible";
      imagenet_class_label[512]= "corkscrew, bottle screw";
      imagenet_class_label[513]= "cornet, horn, trumpet, trump";
      imagenet_class_label[514]= "cowboy boot";
      imagenet_class_label[515]= "cowboy hat, ten-gallon hat";
      imagenet_class_label[516]= "cradle";
      imagenet_class_label[517]= "crane";
      imagenet_class_label[518]= "crash helmet";
      imagenet_class_label[519]= "crate";
      imagenet_class_label[520]= "crib, cot";
      imagenet_class_label[521]= "Crock Pot";
      imagenet_class_label[522]= "croquet ball";
      imagenet_class_label[523]= "crutch";
      imagenet_class_label[524]= "cuirass";
      imagenet_class_label[525]= "dam, dike, dyke";
      imagenet_class_label[526]= "desk";
      imagenet_class_label[527]= "desktop computer";
      imagenet_class_label[528]= "dial telephone, dial phone";
      imagenet_class_label[529]= "diaper, nappy, napkin";
      imagenet_class_label[530]= "digital clock";
      imagenet_class_label[531]= "digital watch";
      imagenet_class_label[532]= "dining table, board";
      imagenet_class_label[533]= "dishrag, dishcloth";
      imagenet_class_label[534]= "dishwasher, dish washer, dishwashing machine";
      imagenet_class_label[535]= "disk brake, disc brake";
      imagenet_class_label[536]= "dock, dockage, docking facility";
      imagenet_class_label[537]= "dogsled, dog sled, dog sleigh";
      imagenet_class_label[538]= "dome";
      imagenet_class_label[539]= "doormat, welcome mat";
      imagenet_class_label[540]= "drilling platform, offshore rig";
      imagenet_class_label[541]= "drum, membranophone, tympan";
      imagenet_class_label[542]= "drumstick";
      imagenet_class_label[543]= "dumbbell";
      imagenet_class_label[544]= "Dutch oven";
      imagenet_class_label[545]= "electric fan, blower";
      imagenet_class_label[546]= "electric guitar";
      imagenet_class_label[547]= "electric locomotive";
      imagenet_class_label[548]= "entertainment center";
      imagenet_class_label[549]= "envelope";
      imagenet_class_label[550]= "espresso maker";
      imagenet_class_label[551]= "face powder";
      imagenet_class_label[552]= "feather boa, boa";
      imagenet_class_label[553]= "file, file cabinet, filing cabinet";
      imagenet_class_label[554]= "fireboat";
      imagenet_class_label[555]= "fire engine, fire truck";
      imagenet_class_label[556]= "fire screen, fireguard";
      imagenet_class_label[557]= "flagpole, flagstaff";
      imagenet_class_label[558]= "flute, transverse flute";
      imagenet_class_label[559]= "folding chair";
      imagenet_class_label[560]= "football helmet";
      imagenet_class_label[561]= "forklift";
      imagenet_class_label[562]= "fountain";
      imagenet_class_label[563]= "fountain pen";
      imagenet_class_label[564]= "four-poster";
      imagenet_class_label[565]= "freight car";
      imagenet_class_label[566]= "French horn, horn";
      imagenet_class_label[567]= "frying pan, frypan, skillet";
      imagenet_class_label[568]= "fur coat";
      imagenet_class_label[569]= "garbage truck, dustcart";
      imagenet_class_label[570]= "gasmask, respirator, gas helmet";
      imagenet_class_label[571]= "gas pump, gasoline pump, petrol pump, island dispenser";
      imagenet_class_label[572]= "goblet";
      imagenet_class_label[573]= "go-kart";
      imagenet_class_label[574]= "golf ball";
      imagenet_class_label[575]= "golfcart, golf cart";
      imagenet_class_label[576]= "gondola";
      imagenet_class_label[577]= "gong, tam-tam";
      imagenet_class_label[578]= "gown";
      imagenet_class_label[579]= "grand piano, grand";
      imagenet_class_label[580]= "greenhouse, nursery, glasshouse";
      imagenet_class_label[581]= "grille, radiator grille";
      imagenet_class_label[582]= "grocery store, grocery, food market, market";
      imagenet_class_label[583]= "guillotine";
      imagenet_class_label[584]= "hair slide";
      imagenet_class_label[585]= "hair spray";
      imagenet_class_label[586]= "half track";
      imagenet_class_label[587]= "hammer";
      imagenet_class_label[588]= "hamper";
      imagenet_class_label[589]= "hand blower, blow dryer, blow drier, hair dryer, hair drier";
      imagenet_class_label[590]= "hand-held computer, hand-held microcomputer";
      imagenet_class_label[591]= "handkerchief, hankie, hanky, hankey";
      imagenet_class_label[592]= "hard disc, hard disk, fixed disk";
      imagenet_class_label[593]= "harmonica, mouth organ, harp, mouth harp";
      imagenet_class_label[594]= "harp";
      imagenet_class_label[595]= "harvester, reaper";
      imagenet_class_label[596]= "hatchet";
      imagenet_class_label[597]= "holster";
      imagenet_class_label[598]= "home theater, home theatre";
      imagenet_class_label[599]= "honeycomb";
      imagenet_class_label[600]= "hook, claw";
      imagenet_class_label[601]= "hoopskirt, crinoline";
      imagenet_class_label[602]= "horizontal bar, high bar";
      imagenet_class_label[603]= "horse cart, horse-cart";
      imagenet_class_label[604]= "hourglass";
      imagenet_class_label[605]= "iPod";
      imagenet_class_label[606]= "iron, smoothing iron";
      imagenet_class_label[607]= "jack-o-lantern";
      imagenet_class_label[608]= "jean, blue jean, denim";
      imagenet_class_label[609]= "jeep, landrover";
      imagenet_class_label[610]= "jersey, T-shirt, tee shirt";
      imagenet_class_label[611]= "jigsaw puzzle";
      imagenet_class_label[612]= "jinrikisha, ricksha, rickshaw";
      imagenet_class_label[613]= "joystick";
      imagenet_class_label[614]= "kimono";
      imagenet_class_label[615]= "knee pad";
      imagenet_class_label[616]= "knot";
      imagenet_class_label[617]= "lab coat, laboratory coat";
      imagenet_class_label[618]= "ladle";
      imagenet_class_label[619]= "lampshade, lamp shade";
      imagenet_class_label[620]= "laptop, laptop computer";
      imagenet_class_label[621]= "lawn mower, mower";
      imagenet_class_label[622]= "lens cap, lens cover";
      imagenet_class_label[623]= "letter opener, paper knife, paperknife";
      imagenet_class_label[624]= "library";
      imagenet_class_label[625]= "lifeboat";
      imagenet_class_label[626]= "lighter, light, igniter, ignitor";
      imagenet_class_label[627]= "limousine, limo";
      imagenet_class_label[628]= "liner, ocean liner";
      imagenet_class_label[629]= "lipstick, lip rouge";
      imagenet_class_label[630]= "Loafer";
      imagenet_class_label[631]= "lotion";
      imagenet_class_label[632]= "loudspeaker, speaker, speaker unit, loudspeaker system, speaker system";
      imagenet_class_label[633]= "loupe, jewelers loupe";
      imagenet_class_label[634]= "lumbermill, sawmill";
      imagenet_class_label[635]= "magnetic compass";
      imagenet_class_label[636]= "mailbag, postbag";
      imagenet_class_label[637]= "mailbox, letter box";
      imagenet_class_label[638]= "maillot";
      imagenet_class_label[639]= "maillot, tank suit";
      imagenet_class_label[640]= "manhole cover";
      imagenet_class_label[641]= "maraca";
      imagenet_class_label[642]= "marimba, xylophone";
      imagenet_class_label[643]= "mask";
      imagenet_class_label[644]= "matchstick";
      imagenet_class_label[645]= "maypole";
      imagenet_class_label[646]= "maze, labyrinth";
      imagenet_class_label[647]= "measuring cup";
      imagenet_class_label[648]= "medicine chest, medicine cabinet";
      imagenet_class_label[649]= "megalith, megalithic structure";
      imagenet_class_label[650]= "microphone, mike";
      imagenet_class_label[651]= "microwave, microwave oven";
      imagenet_class_label[652]= "military uniform";
      imagenet_class_label[653]= "milk can";
      imagenet_class_label[654]= "minibus";
      imagenet_class_label[655]= "miniskirt, mini";
      imagenet_class_label[656]= "minivan";
      imagenet_class_label[657]= "missile";
      imagenet_class_label[658]= "mitten";
      imagenet_class_label[659]= "mixing bowl";
      imagenet_class_label[660]= "mobile home, manufactured home";
      imagenet_class_label[661]= "Model T";
      imagenet_class_label[662]= "modem";
      imagenet_class_label[663]= "monastery";
      imagenet_class_label[664]= "monitor";
      imagenet_class_label[665]= "moped";
      imagenet_class_label[666]= "mortar";
      imagenet_class_label[667]= "mortarboard";
      imagenet_class_label[668]= "mosque";
      imagenet_class_label[669]= "mosquito net";
      imagenet_class_label[670]= "motor scooter, scooter";
      imagenet_class_label[671]= "mountain bike, all-terrain bike, off-roader";
      imagenet_class_label[672]= "mountain tent";
      imagenet_class_label[673]= "mouse, computer mouse";
      imagenet_class_label[674]= "mousetrap";
      imagenet_class_label[675]= "moving van";
      imagenet_class_label[676]= "muzzle";
      imagenet_class_label[677]= "nail";
      imagenet_class_label[678]= "neck brace";
      imagenet_class_label[679]= "necklace";
      imagenet_class_label[680]= "nipple";
      imagenet_class_label[681]= "notebook, notebook computer";
      imagenet_class_label[682]= "obelisk";
      imagenet_class_label[683]= "oboe, hautboy, hautbois";
      imagenet_class_label[684]= "ocarina, sweet potato";
      imagenet_class_label[685]= "odometer, hodometer, mileometer, milometer";
      imagenet_class_label[686]= "oil filter";
      imagenet_class_label[687]= "organ, pipe organ";
      imagenet_class_label[688]= "oscilloscope, scope, cathode-ray oscilloscope, CRO";
      imagenet_class_label[689]= "overskirt";
      imagenet_class_label[690]= "oxcart";
      imagenet_class_label[691]= "oxygen mask";
      imagenet_class_label[692]= "packet";
      imagenet_class_label[693]= "paddle, boat paddle";
      imagenet_class_label[694]= "paddlewheel, paddle wheel";
      imagenet_class_label[695]= "padlock";
      imagenet_class_label[696]= "paintbrush";
      imagenet_class_label[697]= "pajama, pyjama, pjs, jammies";
      imagenet_class_label[698]= "palace";
      imagenet_class_label[699]= "panpipe, pandean pipe, syrinx";
      imagenet_class_label[700]= "paper towel";
      imagenet_class_label[701]= "parachute, chute";
      imagenet_class_label[702]= "parallel bars, bars";
      imagenet_class_label[703]= "park bench";
      imagenet_class_label[704]= "parking meter";
      imagenet_class_label[705]= "passenger car, coach, carriage";
      imagenet_class_label[706]= "patio, terrace";
      imagenet_class_label[707]= "pay-phone, pay-station";
      imagenet_class_label[708]= "pedestal, plinth, footstall";
      imagenet_class_label[709]= "pencil box, pencil case";
      imagenet_class_label[710]= "pencil sharpener";
      imagenet_class_label[711]= "perfume, essence";
      imagenet_class_label[712]= "Petri dish";
      imagenet_class_label[713]= "photocopier";
      imagenet_class_label[714]= "pick, plectrum, plectron";
      imagenet_class_label[715]= "pickelhaube";
      imagenet_class_label[716]= "picket fence, paling";
      imagenet_class_label[717]= "pickup, pickup truck";
      imagenet_class_label[718]= "pier";
      imagenet_class_label[719]= "piggy bank, penny bank";
      imagenet_class_label[720]= "pill bottle";
      imagenet_class_label[721]= "pillow";
      imagenet_class_label[722]= "ping-pong ball";
      imagenet_class_label[723]= "pinwheel";
      imagenet_class_label[724]= "pirate, pirate ship";
      imagenet_class_label[725]= "pitcher, ewer";
      imagenet_class_label[726]= "plane, carpenters plane, woodworking plane";
      imagenet_class_label[727]= "planetarium";
      imagenet_class_label[728]= "plastic bag";
      imagenet_class_label[729]= "plate rack";
      imagenet_class_label[730]= "plow, plough";
      imagenet_class_label[731]= "plunger, plumbers helper";
      imagenet_class_label[732]= "Polaroid camera, Polaroid Land camera";
      imagenet_class_label[733]= "pole";
      imagenet_class_label[734]= "police van, police wagon, paddy wagon, patrol wagon, wagon, black Maria";
      imagenet_class_label[735]= "poncho";
      imagenet_class_label[736]= "pool table, billiard table, snooker table";
      imagenet_class_label[737]= "pop bottle, soda bottle";
      imagenet_class_label[738]= "pot, flowerpot";
      imagenet_class_label[739]= "potters wheel";
      imagenet_class_label[740]= "power drill";
      imagenet_class_label[741]= "prayer rug, prayer mat";
      imagenet_class_label[742]= "printer";
      imagenet_class_label[743]= "prison, prison house";
      imagenet_class_label[744]= "projectile, missile";
      imagenet_class_label[745]= "projector";
      imagenet_class_label[746]= "puck, hockey puck";
      imagenet_class_label[747]= "punching bag, punch bag, punching ball, punchball";
      imagenet_class_label[748]= "purse";
      imagenet_class_label[749]= "quill, quill pen";
      imagenet_class_label[750]= "quilt, comforter, comfort, puff";
      imagenet_class_label[751]= "racer, race car, racing car";
      imagenet_class_label[752]= "racket, racquet";
      imagenet_class_label[753]= "radiator";
      imagenet_class_label[754]= "radio, wireless";
      imagenet_class_label[755]= "radio telescope, radio reflector";
      imagenet_class_label[756]= "rain barrel";
      imagenet_class_label[757]= "recreational vehicle, RV, R.V.";
      imagenet_class_label[758]= "reel";
      imagenet_class_label[759]= "reflex camera";
      imagenet_class_label[760]= "refrigerator, icebox";
      imagenet_class_label[761]= "remote control, remote";
      imagenet_class_label[762]= "restaurant, eating house, eating place, eatery";
      imagenet_class_label[763]= "revolver, six-gun, six-shooter";
      imagenet_class_label[764]= "rifle";
      imagenet_class_label[765]= "rocking chair, rocker";
      imagenet_class_label[766]= "rotisserie";
      imagenet_class_label[767]= "rubber eraser, rubber, pencil eraser";
      imagenet_class_label[768]= "rugby ball";
      imagenet_class_label[769]= "rule, ruler";
      imagenet_class_label[770]= "running shoe";
      imagenet_class_label[771]= "safe";
      imagenet_class_label[772]= "safety pin";
      imagenet_class_label[773]= "saltshaker, salt shaker";
      imagenet_class_label[774]= "sandal";
      imagenet_class_label[775]= "sarong";
      imagenet_class_label[776]= "sax, saxophone";
      imagenet_class_label[777]= "scabbard";
      imagenet_class_label[778]= "scale, weighing machine";
      imagenet_class_label[779]= "school bus";
      imagenet_class_label[780]= "schooner";
      imagenet_class_label[781]= "scoreboard";
      imagenet_class_label[782]= "screen, CRT screen";
      imagenet_class_label[783]= "screw";
      imagenet_class_label[784]= "screwdriver";
      imagenet_class_label[785]= "seat belt, seatbelt";
      imagenet_class_label[786]= "sewing machine";
      imagenet_class_label[787]= "shield, buckler";
      imagenet_class_label[788]= "shoe shop, shoe-shop, shoe store";
      imagenet_class_label[789]= "shoji";
      imagenet_class_label[790]= "shopping basket";
      imagenet_class_label[791]= "shopping cart";
      imagenet_class_label[792]= "shovel";
      imagenet_class_label[793]= "shower cap";
      imagenet_class_label[794]= "shower curtain";
      imagenet_class_label[795]= "ski";
      imagenet_class_label[796]= "ski mask";
      imagenet_class_label[797]= "sleeping bag";
      imagenet_class_label[798]= "slide rule, slipstick";
      imagenet_class_label[799]= "sliding door";
      imagenet_class_label[800]= "slot, one-armed bandit";
      imagenet_class_label[801]= "snorkel";
      imagenet_class_label[802]= "snowmobile";
      imagenet_class_label[803]= "snowplow, snowplough";
      imagenet_class_label[804]= "soap dispenser";
      imagenet_class_label[805]= "soccer ball";
      imagenet_class_label[806]= "sock";
      imagenet_class_label[807]= "solar dish, solar collector, solar furnace";
      imagenet_class_label[808]= "sombrero";
      imagenet_class_label[809]= "soup bowl";
      imagenet_class_label[810]= "space bar";
      imagenet_class_label[811]= "space heater";
      imagenet_class_label[812]= "space shuttle";
      imagenet_class_label[813]= "spatula";
      imagenet_class_label[814]= "speedboat";
      imagenet_class_label[815]= "spider web, spiders web";
      imagenet_class_label[816]= "spindle";
      imagenet_class_label[817]= "sports car, sport car";
      imagenet_class_label[818]= "spotlight, spot";
      imagenet_class_label[819]= "stage";
      imagenet_class_label[820]= "steam locomotive";
      imagenet_class_label[821]= "steel arch bridge";
      imagenet_class_label[822]= "steel drum";
      imagenet_class_label[823]= "stethoscope";
      imagenet_class_label[824]= "stole";
      imagenet_class_label[825]= "stone wall";
      imagenet_class_label[826]= "stopwatch, stop watch";
      imagenet_class_label[827]= "stove";
      imagenet_class_label[828]= "strainer";
      imagenet_class_label[829]= "streetcar, tram, tramcar, trolley, trolley car";
      imagenet_class_label[830]= "stretcher";
      imagenet_class_label[831]= "studio couch, day bed";
      imagenet_class_label[832]= "stupa, tope";
      imagenet_class_label[833]= "submarine, pigboat, sub, U-boat";
      imagenet_class_label[834]= "suit, suit of clothes";
      imagenet_class_label[835]= "sundial";
      imagenet_class_label[836]= "sunglass";
      imagenet_class_label[837]= "sunglasses, dark glasses, shades";
      imagenet_class_label[838]= "sunscreen, sunblock, sun blocker";
      imagenet_class_label[839]= "suspension bridge";
      imagenet_class_label[840]= "swab, swob, mop";
      imagenet_class_label[841]= "sweatshirt";
      imagenet_class_label[842]= "swimming trunks, bathing trunks";
      imagenet_class_label[843]= "swing";
      imagenet_class_label[844]= "switch, electric switch, electrical switch";
      imagenet_class_label[845]= "syringe";
      imagenet_class_label[846]= "table lamp";
      imagenet_class_label[847]= "tank, army tank, armored combat vehicle, armoured combat vehicle";
      imagenet_class_label[848]= "tape player";
      imagenet_class_label[849]= "teapot";
      imagenet_class_label[850]= "teddy, teddy bear";
      imagenet_class_label[851]= "television, television system";
      imagenet_class_label[852]= "tennis ball";
      imagenet_class_label[853]= "thatch, thatched roof";
      imagenet_class_label[854]= "theater curtain, theatre curtain";
      imagenet_class_label[855]= "thimble";
      imagenet_class_label[856]= "thresher, thrasher, threshing machine";
      imagenet_class_label[857]= "throne";
      imagenet_class_label[858]= "tile roof";
      imagenet_class_label[859]= "toaster";
      imagenet_class_label[860]= "tobacco shop, tobacconist shop, tobacconist";
      imagenet_class_label[861]= "toilet seat";
      imagenet_class_label[862]= "torch";
      imagenet_class_label[863]= "totem pole";
      imagenet_class_label[864]= "tow truck, tow car, wrecker";
      imagenet_class_label[865]= "toyshop";
      imagenet_class_label[866]= "tractor";
      imagenet_class_label[867]= "trailer truck, tractor trailer, trucking rig, rig, articulated lorry, semi";
      imagenet_class_label[868]= "tray";
      imagenet_class_label[869]= "trench coat";
      imagenet_class_label[870]= "tricycle, trike, velocipede";
      imagenet_class_label[871]= "trimaran";
      imagenet_class_label[872]= "tripod";
      imagenet_class_label[873]= "triumphal arch";
      imagenet_class_label[874]= "trolleybus, trolley coach, trackless trolley";
      imagenet_class_label[875]= "trombone";
      imagenet_class_label[876]= "tub, vat";
      imagenet_class_label[877]= "turnstile";
      imagenet_class_label[878]= "typewriter keyboard";
      imagenet_class_label[879]= "umbrella";
      imagenet_class_label[880]= "unicycle, monocycle";
      imagenet_class_label[881]= "upright, upright piano";
      imagenet_class_label[882]= "vacuum, vacuum cleaner";
      imagenet_class_label[883]= "vase";
      imagenet_class_label[884]= "vault";
      imagenet_class_label[885]= "velvet";
      imagenet_class_label[886]= "vending machine";
      imagenet_class_label[887]= "vestment";
      imagenet_class_label[888]= "viaduct";
      imagenet_class_label[889]= "violin, fiddle";
      imagenet_class_label[890]= "volleyball";
      imagenet_class_label[891]= "waffle iron";
      imagenet_class_label[892]= "wall clock";
      imagenet_class_label[893]= "wallet, billfold, notecase, pocketbook";
      imagenet_class_label[894]= "wardrobe, closet, press";
      imagenet_class_label[895]= "warplane, military plane";
      imagenet_class_label[896]= "washbasin, handbasin, washbowl, lavabo, wash-hand basin";
      imagenet_class_label[897]= "washer, automatic washer, washing machine";
      imagenet_class_label[898]= "water bottle";
      imagenet_class_label[899]= "water jug";
      imagenet_class_label[900]= "water tower";
      imagenet_class_label[901]= "whiskey jug";
      imagenet_class_label[902]= "whistle";
      imagenet_class_label[903]= "wig";
      imagenet_class_label[904]= "window screen";
      imagenet_class_label[905]= "window shade";
      imagenet_class_label[906]= "Windsor tie";
      imagenet_class_label[907]= "wine bottle";
      imagenet_class_label[908]= "wing";
      imagenet_class_label[909]= "wok";
      imagenet_class_label[910]= "wooden spoon";
      imagenet_class_label[911]= "wool, woolen, woollen";
      imagenet_class_label[912]= "worm fence, snake fence, snake-rail fence, Virginia fence";
      imagenet_class_label[913]= "wreck";
      imagenet_class_label[914]= "yawl";
      imagenet_class_label[915]= "yurt";
      imagenet_class_label[916]= "web site, website, internet site, site";
      imagenet_class_label[917]= "comic book";
      imagenet_class_label[918]= "crossword puzzle, crossword";
      imagenet_class_label[919]= "street sign";
      imagenet_class_label[920]= "traffic light, traffic signal, stoplight";
      imagenet_class_label[921]= "book jacket, dust cover, dust jacket, dust wrapper";
      imagenet_class_label[922]= "menu";
      imagenet_class_label[923]= "plate";
      imagenet_class_label[924]= "guacamole";
      imagenet_class_label[925]= "consomme";
      imagenet_class_label[926]= "hot pot, hotpot";
      imagenet_class_label[927]= "trifle";
      imagenet_class_label[928]= "ice cream, icecream";
      imagenet_class_label[929]= "ice lolly, lolly, lollipop, popsicle";
      imagenet_class_label[930]= "French loaf";
      imagenet_class_label[931]= "bagel, beigel";
      imagenet_class_label[932]= "pretzel";
      imagenet_class_label[933]= "cheeseburger";
      imagenet_class_label[934]= "hotdog, hot dog, red hot";
      imagenet_class_label[935]= "mashed potato";
      imagenet_class_label[936]= "head cabbage";
      imagenet_class_label[937]= "broccoli";
      imagenet_class_label[938]= "cauliflower";
      imagenet_class_label[939]= "zucchini, courgette";
      imagenet_class_label[940]= "spaghetti squash";
      imagenet_class_label[941]= "acorn squash";
      imagenet_class_label[942]= "butternut squash";
      imagenet_class_label[943]= "cucumber, cuke";
      imagenet_class_label[944]= "artichoke, globe artichoke";
      imagenet_class_label[945]= "bell pepper";
      imagenet_class_label[946]= "cardoon";
      imagenet_class_label[947]= "mushroom";
      imagenet_class_label[948]= "Granny Smith";
      imagenet_class_label[949]= "strawberry";
      imagenet_class_label[950]= "orange";
      imagenet_class_label[951]= "lemon";
      imagenet_class_label[952]= "fig";
      imagenet_class_label[953]= "pineapple, ananas";
      imagenet_class_label[954]= "banana";
      imagenet_class_label[955]= "jackfruit, jak, jack";
      imagenet_class_label[956]= "custard apple";
      imagenet_class_label[957]= "pomegranate";
      imagenet_class_label[958]= "hay";
      imagenet_class_label[959]= "carbonara";
      imagenet_class_label[960]= "chocolate sauce, chocolate syrup";
      imagenet_class_label[961]= "dough";
      imagenet_class_label[962]= "meat loaf, meatloaf";
      imagenet_class_label[963]= "pizza, pizza pie";
      imagenet_class_label[964]= "potpie";
      imagenet_class_label[965]= "burrito";
      imagenet_class_label[966]= "red wine";
      imagenet_class_label[967]= "espresso";
      imagenet_class_label[968]= "cup";
      imagenet_class_label[969]= "eggnog";
      imagenet_class_label[970]= "alp";
      imagenet_class_label[971]= "bubble";
      imagenet_class_label[972]= "cliff, drop, drop-off";
      imagenet_class_label[973]= "coral reef";
      imagenet_class_label[974]= "geyser";
      imagenet_class_label[975]= "lakeside, lakeshore";
      imagenet_class_label[976]= "promontory, headland, head, foreland";
      imagenet_class_label[977]= "sandbar, sand bar";
      imagenet_class_label[978]= "seashore, coast, seacoast, sea-coast";
      imagenet_class_label[979]= "valley, vale";
      imagenet_class_label[980]= "volcano";
      imagenet_class_label[981]= "ballplayer, baseball player";
      imagenet_class_label[982]= "groom, bridegroom";
      imagenet_class_label[983]= "scuba diver";
      imagenet_class_label[984]= "rapeseed";
      imagenet_class_label[985]= "daisy";
      imagenet_class_label[986]= "yellow ladys slipper, yellow lady-slipper, Cypripedium calceolus, Cypripedium parviflorum";
      imagenet_class_label[987]= "corn";
      imagenet_class_label[988]= "acorn";
      imagenet_class_label[989]= "hip, rose hip, rosehip";
      imagenet_class_label[990]= "buckeye, horse chestnut, conker";
      imagenet_class_label[991]= "coral fungus";
      imagenet_class_label[992]= "agaric";
      imagenet_class_label[993]= "gyromitra";
      imagenet_class_label[994]= "stinkhorn, carrion fungus";
      imagenet_class_label[995]= "earthstar";
      imagenet_class_label[996]= "hen-of-the-woods, hen of the woods, Polyporus frondosus, Grifola frondosa";
      imagenet_class_label[997]= "bolete";
      imagenet_class_label[998]= "ear, spike, capitulum";
      imagenet_class_label[999]= "toilet tissue, toilet paper, bathroom tissue";

	int ercode = 0;

	//set memory allocation to tensor_arena
	tensor_arena = mm_reserve_align(tensor_arena_size,0x20); //1mb
	xprintf("TA[%x]\r\n",tensor_arena);


	if(_arm_npu_init(security_enable, privilege_enable)!=0)
		return -1;

	if(model_addr != 0) {
		static const tflite::Model*eff_lite0_cls_model = tflite::GetModel((const void *)model_addr);

		if (eff_lite0_cls_model->version() != TFLITE_SCHEMA_VERSION) {
			xprintf(
				"[ERROR] eff_lite0_cls_model's schema version %d is not equal "
				"to supported version %d\n",
				eff_lite0_cls_model->version(), TFLITE_SCHEMA_VERSION);
			return -1;
		}
		else {
			xprintf("eff_lite0_cls_model model's schema version %d\n", eff_lite0_cls_model->version());
		}

		static tflite::MicroErrorReporter eff_lite0_cls_micro_error_reporter;
		static tflite::MicroMutableOpResolver<1> eff_lite0_cls_op_resolver;

		if (kTfLiteOk != eff_lite0_cls_op_resolver.AddEthosU()){
			xprintf("Failed to add Arm NPU support to op resolver.");
			return false;
		}

		static tflite::MicroInterpreter eff_lite0_cls_static_interpreter(eff_lite0_cls_model, eff_lite0_cls_op_resolver,
				(uint8_t*)tensor_arena, tensor_arena_size, &eff_lite0_cls_micro_error_reporter);

		if(eff_lite0_cls_static_interpreter.AllocateTensors()!= kTfLiteOk) {
			return false;
		}
		eff_lite0_cls_int_ptr = &eff_lite0_cls_static_interpreter;
		eff_lite0_cls_input = eff_lite0_cls_static_interpreter.input(0);
		eff_lite0_cls_output = eff_lite0_cls_static_interpreter.output(0);
	}

	xprintf("initial done\n");
	return ercode;
}


static void efficientnet_lite0_class_post_processing(tflite::MicroInterpreter* static_interpreter)
{
	uint32_t img_w = app_get_raw_width();
    uint32_t img_h = app_get_raw_height();
	TfLiteTensor* output = static_interpreter->output(0);
	float output_scale = ((TfLiteAffineQuantization*)(output->quantization.params))->scale->data[0];
	int output_zeropoint = ((TfLiteAffineQuantization*)(output->quantization.params))->zero_point->data[0];
	int output_size = output->bytes;
	uint16_t maxClassIndex = 0;
	float max_deq_value = -1000;
	for(int idx = 0; idx < 1000; idx++)
	{

		int value =  output->data.int8[idx];
			
		float deq_value = ((float) value-(float)output_zeropoint) * output_scale ;

		if(deq_value >= max_deq_value)
		{
			max_deq_value = deq_value;
			maxClassIndex = idx;
		}
	}
	printf("imagenet class id[%d]  %s score: %d\r\n",maxClassIndex, imagenet_class_label[maxClassIndex].c_str(),int(max_deq_value*100));
}



int cv_efficientnet_lite0_class_run() {
	int ercode = 0;
    float w_scale;
    float h_scale;
    uint32_t img_w = app_get_raw_width();
    uint32_t img_h = app_get_raw_height();
    uint32_t ch = app_get_raw_channels();
    uint32_t raw_addr = app_get_raw_addr();
    uint32_t expand = 0;


	#if DBG_APP_LOG
    xprintf("raw info: w[%d] h[%d] ch[%d] addr[%x]\n",img_w, img_h, ch, raw_addr);
	#endif

    if(eff_lite0_cls_int_ptr!= nullptr) {
    	//get image from sensor and resize
		w_scale = (float)(img_w - 1) / (EFF_LITE0_CLS_INPUT_TENSOR_WIDTH - 1);
		h_scale = (float)(img_h - 1) / (EFF_LITE0_CLS_INPUT_TENSOR_HEIGHT - 1);

		
		hx_lib_image_resize_BGR8U3C_to_RGB24_helium((uint8_t*)raw_addr, (uint8_t*)eff_lite0_cls_input->data.data,  
		                    img_w, img_h, ch, 
                        	EFF_LITE0_CLS_INPUT_TENSOR_WIDTH, EFF_LITE0_CLS_INPUT_TENSOR_HEIGHT, w_scale,h_scale);

        #if 0   // send RGB24 image for debug
		uint8_t imagesize_header[4];
		imagesize_header[0] = EFF_LITE0_CLS_INPUT_TENSOR_WIDTH & 0xFF;
		imagesize_header[1] = (EFF_LITE0_CLS_INPUT_TENSOR_WIDTH>>8) & 0xFF;
		imagesize_header[2] = EFF_LITE0_CLS_INPUT_TENSOR_HEIGHT & 0xFF;
		imagesize_header[3] = (EFF_LITE0_CLS_INPUT_TENSOR_HEIGHT>>8) & 0xFF;
        hx_drv_spi_mst_protocol_write_ex((uint32_t)eff_lite0_cls_input->data.data, EFF_LITE0_CLS_INPUT_TENSOR_WIDTH*EFF_LITE0_CLS_INPUT_TENSOR_HEIGHT*ch, DATA_TYPE_RAW_HEADER_IMG_RGB_1PIX, imagesize_header, 4);
        #endif

		// //uint8 to int8
		for (int i = 0; i < eff_lite0_cls_input->bytes; ++i) {
			*((int8_t *)eff_lite0_cls_input->data.data+i) = *((int8_t *)eff_lite0_cls_input->data.data+i) - 128;
    	}

        #if INVOKE_STEP_TICK
		SystemGetTick(&systick_1, &loop_cnt_1);
        #endif

		TfLiteStatus invoke_status = eff_lite0_cls_int_ptr->Invoke();

        #if INVOKE_STEP_TICK
		SystemGetTick(&systick_2, &loop_cnt_2);
		xprintf("Tick for invoke of efficentnet lite0 classification:[%d]\r\n",(loop_cnt_2-loop_cnt_1)*CPU_CLK+(systick_1-systick_2));	
        #endif

		if(invoke_status != kTfLiteOk)
		{
			xprintf("efficentnet_lite0 classification invoke fail\n");
			return -1;
		}
		else
		{
			#if DBG_APP_LOG
			xprintf("efficentnet_lite0 classification invoke pass\n");
			#endif
		}
		//retrieve output data
		efficientnet_lite0_class_post_processing(eff_lite0_cls_int_ptr);

		#if DBG_APP_LOG
			xprintf("efficentnet_lite0 classification done\r\n");
		#endif
    }
	sensordplib_retrigger_capture();
	return ercode;
}

int cv_efficientnet_lite0_class_deinit()
{
	
	return 0;
}

