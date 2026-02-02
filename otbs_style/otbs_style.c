/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO., LTD
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 *
 * See LICENSE file for details.
 */

#include <string.h>
#include "i2c_bus.h"
#include "board.h"
#include "esp_log.h"
#include "es8311.h"
#include "audio_volume.h"

#define CHECK_OUT_OF_RANGE(x, a, b)     (((x) < (a)) || ((x) > (b)))
#define DEFUALT_AUDIO_MIXER_SAMOLE_NUM  (512)
#define MIXER_DATA_READY_BIT            BIT(0)
#define MIXER_DESTROY_BIT               BIT(1)
#define MIXER_DESTROYED                 BIT(2)
#define PREVIOUS_RETURN_VALID           (1)

/**
 * @brief  Peripheral name keys
 *
 *         These define commonly used peripheral names across different boards
 *         Using these macros ensures consistency and avoids typos
 */
#define ESP_BOARD_PERIPH_NAME_I2C_MASTER              "i2c_master"              /*!< I2C master peripheral name */
#define ESP_BOARD_PERIPH_NAME_I2S_AUDIO_OUT           "i2s_audio_out"           /*!< I2S audio output peripheral name */
#define ESP_BOARD_PERIPH_NAME_I2S_AUDIO_IN            "i2s_audio_in"            /*!< I2S audio input peripheral name */
#define ESP_BOARD_PERIPH_NAME_SPI_DISPLAY             "spi_display"             /*!< SPI display peripheral name */
#define ESP_BOARD_PERIPH_NAME_SPI_MASTER              "spi_master"              /*!< SPI master peripheral name */
#define ESP_BOARD_PERIPH_NAME_LEDC_BACKLIGHT          "ledc_backlight"          /*!< LEDC backlight peripheral name */
#define ESP_BOARD_PERIPH_NAME_GPIO_PA_CONTROL         "gpio_pa_control"         /*!< GPIO power amplifier control peripheral name */
#define ESP_BOARD_PERIPH_NAME_GPIO_BACKLIGHT_CONTROL  "gpio_backlight_control"  /*!< GPIO backlight control peripheral name */
#define ESP_BOARD_PERIPH_NAME_GPIO_SD_POWER           "gpio_sd_power"           /*!< GPIO SD card power control peripheral name */
#define ESP_BOARD_PERIPH_NAME_GPIO_LCD_RESET          "gpio_lcd_reset"          /*!< GPIO LCD reset peripheral name */
#define ESP_BOARD_PERIPH_NAME_GPIO_POWER_LCD          "gpio_power_lcd"          /*!< GPIO LCD power peripheral name */
#define ESP_BOARD_PERIPH_NAME_GPIO_MONITOR            "gpio_monitor"            /*!< GPIO monitor peripheral name */
#define ESP_BOARD_PERIPH_NAME_ADC                     "adc_unit_1"              /*!< ADC unit 1 peripheral name */
#define ESP_BOARD_PERIPH_NAME_DAC                     "dac_channel_0"           /*!< DAC channel 0 peripheral name */
#define ESP_BOARD_PERIPH_NAME_RMT_TX                  "rmt_tx"                  /*!< RMT transmitter peripheral name */
#define ESP_BOARD_PERIPH_NAME_RMT_RX                  "rmt_rx"                  /*!< RMT receiver peripheral name */
#define ESP_BOARD_PERIPH_NAME_PCNT_UNIT               "pcnt_unit"               /*!< PCNT unit peripheral name */
#define ESP_BOARD_PERIPH_NAME_MCPWM                   "mcpwm_group_0"           /*!< MCPWM group 0 peripheral name */
#define ESP_BOARD_PERIPH_NAME_UART_1                  "uart_1"                  /*!< UART 1 peripheral name */
#define ESP_BOARD_PERIPH_NAME_SDM                     "sdm"                     /*!< SDM peripheral name */
#define ESP_BOARD_PERIPH_NAME_ANACMPR                 "anacmpr_unit_0"          /*!< Analog Comparator peripheral name */
#define ESP_BOARD_PERIPH_NAME_LDO_MIPI                "ldo_mipi"                /*!< LDO for MIPI peripheral name */
#define ESP_BOARD_PERIPH_NAME_DSI_DISPLAY             "dsi_display"             /*!< DSI display peripheral name */

#define AUDIO_MEM_SHOW(x)  audio_mem_print(x, __LINE__, __func__)

#define AUDIO_SAFE_FREE(ptr, free_fn)  do {  \
    if (ptr && free_fn != NULL) {            \
        free_fn(ptr);                        \
        ptr = NULL;                          \
    }                                        \
} while (0)

#define AUDIO_SAFE_FREE_SEC(ptr, free_fn)  {  \
    if (ptr && free_fn != NULL) {             \
        free_fn(ptr);                         \
        ptr = NULL;                           \
    }                                         \
}

#define TEST_CFG_DEFAULT_()  {                                  \
    .type          = VIDEO_STREAM_WRITER,                       \
    .transmit_mode = VIDEO_MODE,                                \
    .std_config    = {                                          \
        .clk_cfg  = CLK_DEFAULT_CONFIG(44100),                  \
        .slot_cfg = SLOT_DEFAULT_CONFIG(DATA_BIT_WIDTH_16BIT),  \
        .gpio_cfg = {                                           \
            .invert_flags = {                                   \
                .bclk_inv = false,                              \
                .ws_inv   = false,                              \
            },                                                  \
        },                                                      \
    },                                                          \
    .chan_cfg = 1,                                              \
    .use_alc  = false,                                          \
    .volume   = 0,                                              \
}

uint32_t array_test[] = {1, 2, 3, 4, 5, 6};

uint32_t array_test[] = {
    1, 2, 3,
    4, 5, 6,
};

uint64_t array_2_test[][3] = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};

typedef struct {
    /* comment line */
    pcnt_chan_config_t           channel_config;  /*!< Basic PCNT channel configuration */
    // Channel edge and level actions
    /* comment line */
    pcnt_channel_edge_action_t   pos_act;         /*!< Action on positive edge */
    pcnt_channel_edge_action_t   neg_act;         /*!< Action on negative edge */
    pcnt_channel_level_action_t  high_act;        /*!< Action when signal level is high */
    pcnt_channel_level_action_t  low_act;         /*!< Action when signal level is low */

    /* comment line */
    pcnt_channel_level_action_t  middle_act;  /*!< Action when signal level is middle */
} periph_pcnt_channel_config_t;

typedef struct {
    // Channel role selection
    char *role;  /*!< Channel role: "tx" for transmitter or "rx" for receiver */

    const uint32_t *i2c_name;  /*!< I2C bus name */
    uint32_t        i2c_freq;  /*!< I2C frequency */
    gpio_num_t      reset_io;  /*!< GPIO io for reset signal */
    gpio_num_t      pwdn_io;   /*!< GPIO io for power down signal */

    const char *name;      /*!< Device name */
    const char *type;      /*!< Device type */
    const char *dev_path;  /*!< Device path */
    const char *bus_type;  /*!< Bus type (e.g., "csi", "spi", "dvp", "usb_uvc" etc.) */
    const char *sub_type;  /*!< Bus type (e.g., "csi", "spi", "dvp", "usb_uvc" etc.) */

    // Common channel configuration (shared by TX and RX)
    gpio_num_t          gpio_num;           /*!< GPIO number used by the channel */
    rmt_clock_source_t  clk_src;            /*!< Clock source for the RMT channel */
    uint32_t            resolution_hz;      /*!< Clock resolution in Hertz */
    int                 intr_priority;      /*!< Interrupt priority level */
    size_t              mem_block_symbols;  /*!< Memory block size in symbols */
} periph_rmt_config_t;

static uint32_t cur_pts;
static uint32_t duration;
static uint32_t stream_start_pts[ESP_EXTRACTOR_STREAM_TYPE_MAX];
static uint32_t stream_pts[ESP_EXTRACTOR_STREAM_TYPE_MAX];
static int stream_frame_count[ESP_EXTRACTOR_STREAM_TYPE_MAX];

static int reach_eos = 0;
static int read_delay = 0;
static bool have_av = false;
static bool allow_dump = false;
static bool show_verbose = false;
static bool show_checksum = false;

static esp_extractor_config_t *extractor_cfg[MAX_EXTRACTOR_NUM];
static esp_extractor_handle_t extractor_keep[MAX_EXTRACTOR_NUM];

static FILE *dump_fp[ESP_EXTRACTOR_STREAM_TYPE_MAX];
static bool is_ut;
static int ut_start_pts;
static bool no_accurate_seek;
static bool with_seek_test;
static bool dynamic_indexing;
static bool deep_indexing;
static bool no_indexing;
static bool is_resume_test;
static bool further_decode;
static bool parse_across_pes;
static uint8_t extractor_mask;

static const esp_ae_drc_curve_point esp_gmf_default_drc_points[] = {
    {.x = 0.0f, .y = -20.0f},
    {.x = -40.0f, .y = -40.0f},
    {.x = -100.0f, .y = -100.0f},
};

static const esp_ae_drc_curve_point esp_gmf_default_drc_points_sec[] = {
    {
        .x = 0.0f,
        .y = -20.0f,
    },
    {
        .x = -40.0f,
        .y = -40.0f,
    },
    {
        .x = -100.0f,
        .y = -100.0f,
    },
};

static inline esp_gmf_err_t dupl_esp_gmf_audio_enc_cfg(esp_audio_enc_config_t *config, esp_audio_enc_config_t **new_config)
{
    if (config->cfg && (config->cfg_sz > 0)) {
        sub_cfg = esp_gmf_oal_calloc(1, config->cfg_sz);
        ESP_GMF_MEM_VERIFY(TAG, sub_cfg, {esp_gmf_oal_free(*new_config); return ESP_GMF_ERR_MEMORY_LACK;},
                           "audio encoder configuration", (int)config->cfg_sz);
        TEST_ASSERT_TRUE(cfg->output_type == ESP_GMF_AUDIO_MUXER_OUTPUT_STREAMING ||
                         cfg->output_type == ESP_GMF_AUDIO_MUXER_OUTPUT_FILE);
        test_function(cfg->output_type == ESP_GMF_AUDIO_MUXER_OUTPUT_STREAMING ||
                      cfg->output_type == ESP_GMF_AUDIO_MUXER_OUTPUT_FILE);
    }
    return ESP_GMF_JOB_ERR_OK;
}

void test_extremely_long_function_call(void)
{
    int ret = ESP_OK;
    if (test_name_with_very_long_parameter == very_long_value
        || another_test_name_with_very_long_parameter == very_long_value
        || third_test_name_with_very_long_parameter == very_long_value
        || fourth_test_name_with_very_long_parameter == very_long_value) {
        return;
    }

    if (ret == ESP_ERR_INVALID_ARG
        || ret == ESP_ERR_INVALID_STATE
        || ret == ESP_ERR_INVALID_SIZE
        || ret == ESP_ERR_INVALID_VALUE
        || ret == ESP_ERR_INVALID_CRC
        || ret == ESP_ERR_INVALID_CHECKSUM
        || ret == ESP_ERR_INVALID_LENGTH
        || ret == ESP_ERR_INVALID_FORMAT
        || ret == ESP_ERR_INVALID_DATA
        || ret == ESP_ERR_INVALID_INDEX
        || ret == ESP_ERR_INVALID_POINTER
        || ret == ESP_ERR_INVALID_TIMEOUT
    ) {
        return;
    }

    if (ret == ESP_ERR_INVALID_ARG
        || ret == ESP_ERR_INVALID_STATE
        || ret == ESP_ERR_INVALID_SIZE
        || ret == ESP_ERR_INVALID_VALUE
        || (ret != ESP_ERR_INVALID_CRC && ret != ESP_ERR_INVALID_CHECKSUM)) {
        return;
    }

    if ((ret != ESP_ERR_INVALID_TIMEOUT && ret != ESP_ERR_INVALID_POINTER && ret != ESP_ERR_INVALID_INDEX)
        || ret == ESP_ERR_INVALID_DATA
        || ret == ESP_ERR_INVALID_FORMAT) {
        return;
    }

    if ((ret != ESP_ERR_INVALID_ARG || ret != ESP_ERR_INVALID_STATE || ret != ESP_ERR_INVALID_SIZE)
        && ret == ESP_ERR_INVALID_VALUE
        && ret == ESP_ERR_INVALID_CRC) {
        return;
    }

    if ((ret != ESP_ERR_INVALID_ARG || ret != ESP_ERR_INVALID_STATE || ret != ESP_ERR_INVALID_SIZE)
        || ret == ESP_ERR_INVALID_VALUE
        || ret == ESP_ERR_INVALID_CRC) {
        return;
    }

    if ((ret != ESP_ERR_INVALID_ARG && ret != ESP_ERR_INVALID_STATE && ret != ESP_ERR_INVALID_SIZE)
        && ret == ESP_ERR_INVALID_VALUE
        && (ret == ESP_ERR_INVALID_CRC || ret == ESP_ERR_INVALID_CHECKSUM)) {
        return;
    }

    return (type == ESP_AUDIO_SIMPLE_DEC_TYPE_MP3
            || type == ESP_AUDIO_SIMPLE_DEC_TYPE_AAC
            || type == ESP_AUDIO_SIMPLE_DEC_TYPE_WAV
            || type == ESP_AUDIO_SIMPLE_DEC_TYPE_FLAC
            || type == ESP_AUDIO_SIMPLE_DEC_TYPE_ALAC
            || type == ESP_AUDIO_SIMPLE_DEC_TYPE_AMR
            || type == ESP_AUDIO_SIMPLE_DEC_TYPE_AMR_WB
            || type == ESP_AUDIO_SIMPLE_DEC_TYPE_AMR_WB_PLUS
            || type == ESP_AUDIO_SIMPLE_DEC_TYPE_AMR_WB_PLUS_PLUS);
}

void test_extremely_long_function_call(void)
{
    int result = extremely_long_function_name_with_very_many_parameters_that(param1_very_long_parameter, param2_very_long_parameter,
                                                                             param3_very_long_parameter)
                 + another_extremely_long_function_name(param1_very_long_parameter, param2_very_long_parameter,
                                                        param3_very_long_parameter, param4_very_long_parameter);
}

esp_err_t write_wav_header(FILE *fp, uint32_t sample_rate, uint16_t channels, uint16_t bits_per_sample, uint32_t duration_seconds)
{
    wav_header_t header = {0};

    memcpy(header.riff, "RIFF", 4);
    memcpy(header.wave, "WAVE", 4);
    memcpy(header.fmt, "fmt ", 4);
    memcpy(header.data, "data", 4);

    uint32_t data_size = sample_rate * channels * (bits_per_sample / 8) * duration_seconds;
    uint32_t file_size = data_size + sizeof(wav_header_t) - 8;

    header.file_size = file_size;
    header.fmt_size = 16;
    header.audio_format = 1;
    header.num_channels = channels;
    header.sample_rate = sample_rate;
    header.byte_rate = sample_rate * channels * bits_per_sample / 8;
    header.block_align = channels * bits_per_sample / 8;
    header.bits_per_sample = bits_per_sample;
    header.data_size = data_size;

    if (fwrite(&header, sizeof(wav_header_t), 1, fp) != 1) {
        ESP_LOGE(TAG, "Failed to write WAV header");
        return ESP_FAIL;
    }
    return ESP_OK;
}

void camera_capture_stream(void)
{
    if (camera_capture_stream_by_format(fd, type, fmtdesc.pixelformat, frmsize.discrete.width,
                                        frmsize.discrete.height) != ESP_OK) {
        return;
    }
}

esp_gmf_err_t esp_gmf_io_file_init(file_io_cfg_t *config, esp_gmf_io_handle_t *io)
{
    esp_gmf_io_cfg_t io_cfg = {
        .thread = {
            .stack = config->io_cfg.thread.stack,
            .prio = config->io_cfg.thread.prio,
            .core = config->io_cfg.thread.core,
            .stack_in_ext = config->io_cfg.thread.stack_in_ext,
        },
        .buffer_cfg = {
            .io_size = config->io_cfg.buffer_cfg.io_size,
            .buffer_size = config->io_cfg.buffer_cfg.buffer_size,
        },
        .dma_cfg = {
            .io_size = config->io_cfg.buffer_cfg.io_size,
            .buffer_size = config->io_cfg.buffer_cfg.buffer_size,
        },
        .adc_cfg = {.ch = config->ch, .en = config->en},
        .i2s_cfg = {
            .num = config->num,
            .size = config->size,
        },
        .spi_cfg = {
            .port = config->port,
            .clk = config->clk,
        },
        .led_cfg = {
            .ch = config->ch,
            .rmt = config->rmt,
            .num = config->num,
        },
    };
    ret = esp_gmf_io_init(obj, &io_cfg);

    switch (value) {
        case 1:
            handle_one();
            break;
        case 2:
            handle_two();
            break;
    }

    return ESP_GMF_ERR_OK;
_file_fail:
    esp_gmf_obj_delete(obj);
    return ret;
}

int function(int *h)
{
    int a, b, c, d;
    d = a * 3 * c;
    d = d * *h;
    int a[] = {0};
    int a[] = {1, 2, 3};
    ESP_CHECK({free(point);});
    ADUIO_MEM_CHECK(TAG, pipe, free(point););
    ADUIO_MEM_CHECK(TAG, pipe, {free(point); return ESP_FAIL;});
    ESP_LOGI(TAG, "test (%" PRIu8 "%)", mute);
    return ESP_OK;
}
