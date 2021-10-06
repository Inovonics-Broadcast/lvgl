/**
 * @file lv_linearscale.c
 *
 */

 /*********************
  *      INCLUDES
  *********************/
#include "lv_linearscale.h"
#if LV_USE_LINEARSCALE != 0

#include "../lv_misc/lv_debug.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_core/lv_group.h"
#include "../lv_misc/lv_math.h"
#include "../lv_misc/lv_utils.h"

  /*********************
   *      DEFINES
   *********************/
#define LV_OBJX_NAME "lv_linearscale"

   /**********************
	*      TYPEDEFS
	**********************/

	/**********************
	 *  STATIC PROTOTYPES
	 **********************/
static lv_design_res_t lv_linearscale_design(lv_obj_t * lscale, const lv_area_t * clip_area, lv_design_mode_t mode);
static lv_style_list_t * lv_linearscale_get_style(lv_obj_t * lscale, uint8_t part);
static lv_res_t lv_linearscale_signal(lv_obj_t * lscale, lv_signal_t sign, void * param);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_cb_t ancestor_signal;

/**********************
 *      MACROS
 **********************/

 /**********************
  *   GLOBAL FUNCTIONS
  **********************/

  /**
   * Create a linear scale object
   * @param par pointer to an object, it will be the parent of the new linear scale
   * @param copy pointer to a linear scale object, if not NULL then the new object will be copied from
   * it
   * @return pointer to the created linear scale
   */
lv_obj_t * lv_linearscale_create(lv_obj_t * par, const lv_obj_t * copy)
{
	LV_LOG_TRACE("linear scale create started");

	/*Create the ancestor of linear scale*/
	lv_obj_t * linearscale = lv_obj_create(par, copy);
	LV_ASSERT_MEM(linearscale);
	if (linearscale == NULL) return NULL;

	if (ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(linearscale);

	/*Allocate the linear scale type specific extended data*/
	lv_linearscale_ext_t * ext = lv_obj_allocate_ext_attr(linearscale, sizeof(lv_linearscale_ext_t));
	LV_ASSERT_MEM(ext);
	if (ext == NULL) {
		lv_obj_del(linearscale);
		return NULL;
	}

	/*Initialize the allocated 'ext' */
	ext->min_value = 0;
	ext->max_value = 100;
	ext->cur_value = 0;
	ext->line_cnt = 26;
	ext->label_cnt = 6;
	ext->align = LV_LINEARSCALE_ALIGN_LEFT;
	ext->format_cb = NULL;

	/*The signal and design functions are not copied so set them here*/
	lv_obj_set_signal_cb(linearscale, lv_linearscale_signal);
	lv_obj_set_design_cb(linearscale, lv_linearscale_design);

	/*Init the new linear scale*/
	if (copy == NULL) {
		lv_obj_set_size(linearscale, 3 * LV_DPI / 2, 3 * LV_DPI / 2);
		lv_obj_set_click(linearscale, false);
		lv_theme_apply(linearscale, LV_THEME_LINEARSCALE);
	}
	/*Copy an existing linear scale*/
	else {
		lv_linearscale_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
		ext->line_cnt = copy_ext->line_cnt;
		ext->label_cnt = copy_ext->label_cnt;
		ext->min_value = copy_ext->min_value;
		ext->max_value = copy_ext->max_value;
		ext->cur_value = copy_ext->cur_value;
		ext->align = copy_ext->align;
		ext->format_cb = copy_ext->format_cb;

		/*Refresh the style with new signal function*/
		lv_obj_refresh_style(linearscale, LV_OBJ_PART_ALL, LV_STYLE_PROP_ALL);
	}

	LV_LOG_INFO("linear scale created");

	return linearscale;
}

/*=====================
 * Setter functions
 *====================*/

 /**
  * Set a new value on the linear scale
  * @param lscale pointer to a linear scale object
  * @param value new value
  */
void lv_linearscale_set_value(lv_obj_t * lscale, int32_t value)
{
	LV_ASSERT_OBJ(lscale, LV_OBJX_NAME);

	lv_linearscale_ext_t * ext = lv_obj_get_ext_attr(lscale);
	if (ext->cur_value == value) return;

	ext->cur_value = value > ext->max_value ? ext->max_value : value;
	ext->cur_value = ext->cur_value < ext->min_value ? ext->min_value : ext->cur_value;
	lv_obj_invalidate(lscale);
}

/**
 * Set minimum and the maximum values of a linear scale
 * @param lscale pointer to the linear scale object
 * @param min minimum value
 * @param max maximum value
 */
void lv_linearscale_set_range(lv_obj_t * lscale, int32_t min, int32_t max)
{
	LV_ASSERT_OBJ(lscale, LV_OBJX_NAME);

	lv_linearscale_ext_t * ext = lv_obj_get_ext_attr(lscale);
	if (ext->min_value == min && ext->max_value == max) return;

	ext->max_value = max;
	ext->min_value = min;
	if (ext->cur_value > max) {
		ext->cur_value = max;
		lv_linearscale_set_value(lscale, ext->cur_value);
	}
	if (ext->cur_value < min) {
		ext->cur_value = min;
		lv_linearscale_set_value(lscale, ext->cur_value);
	}
	lv_obj_invalidate(lscale);
}

/**
 * Set the number of lines for a linear scale
 * @param lscale pointer to a linear scale object
 * @param line_cnt number of lines
 * @param label_cnt number of labels
 */
void lv_linearscale_set_scale(lv_obj_t * lscale, uint16_t line_cnt, uint16_t label_cnt)
{
	LV_ASSERT_OBJ(lscale, LV_OBJX_NAME);

	if (line_cnt == 0) return;

	lv_linearscale_ext_t * ext = lv_obj_get_ext_attr(lscale);
	if (ext->line_cnt == line_cnt && ext->label_cnt == label_cnt) return;

	ext->line_cnt = line_cnt;
	ext->label_cnt = label_cnt;

	lv_obj_invalidate(lscale);
}

/**
 * Set the alignment side for a linear scale
 * @param lscale pointer to a linear scale object
 * @param dir alignment direction
 */
void lv_linearscale_set_alignment(lv_obj_t * lscale, lv_linearscale_align_t dir)
{
	LV_ASSERT_OBJ(lscale, LV_OBJX_NAME);

	lv_linearscale_ext_t * ext = lv_obj_get_ext_attr(lscale);
	if (ext->align == dir) return;

	ext->align = dir;

	lv_obj_invalidate(lscale);
}

/**
 * Assign a function to format scale values
 * @param lscale pointer to a linear scale object
 * @param format_cb pointer to function of lv_linearscale_format_cb_t
 */
void lv_linearscale_set_formatter_cb(lv_obj_t * lscale, lv_linearscale_format_cb_t format_cb)
{
	LV_ASSERT_OBJ(lscale, LV_OBJX_NAME);

	lv_linearscale_ext_t * ext = lv_obj_get_ext_attr(lscale);

	ext->format_cb = format_cb;
}

/*=====================
 * Getter functions
 *====================*/

 /**
  * Get the value of a linear scale
  * @param lscale pointer to a linear scale object
  * @return the value of the linear scale
  */
int32_t lv_linearscale_get_value(const lv_obj_t * lscale)
{
	LV_ASSERT_OBJ(lscale, LV_OBJX_NAME);

	lv_linearscale_ext_t * ext = lv_obj_get_ext_attr(lscale);
	return ext->cur_value;
}

/**
 * Get the minimum value of a linear scale
 * @param lscale pointer to a linear scale object
 * @return the minimum value of the linear scale
 */
int32_t lv_linearscale_get_min_value(const lv_obj_t * lscale)
{
	LV_ASSERT_OBJ(lscale, LV_OBJX_NAME);

	lv_linearscale_ext_t * ext = lv_obj_get_ext_attr(lscale);
	return ext->min_value;
}

/**
 * Get the maximum value of a linear scale
 * @param lscale pointer to a linear scale object
 * @return the maximum value of the linear scale
 */
int32_t lv_linearscale_get_max_value(const lv_obj_t * lscale)
{
	LV_ASSERT_OBJ(lscale, LV_OBJX_NAME);

	lv_linearscale_ext_t * ext = lv_obj_get_ext_attr(lscale);
	return ext->max_value;
}

/**
 * Get the scale number of a linear scale
 * @param lmeter pointer to a linear scale object
 * @return number of lines
 */
uint16_t lv_linearscale_get_line_count(const lv_obj_t * lscale)
{
	LV_ASSERT_OBJ(lscale, LV_OBJX_NAME);

	lv_linearscale_ext_t * ext = lv_obj_get_ext_attr(lscale);
	return ext->line_cnt;
}

/**
 * Get the scale number of a linear scale
 * @param lscale pointer to a linear scale object
 * @return number of labels
 */
uint16_t lv_linearscale_get_label_count(const lv_obj_t * lscale)
{
	LV_ASSERT_OBJ(lscale, LV_OBJX_NAME);

	lv_linearscale_ext_t * ext = lv_obj_get_ext_attr(lscale);
	return ext->label_cnt;
}

void lv_linearscale_draw_scale(lv_obj_t * lscale, const lv_area_t * clip_area, uint8_t part)
{
	lv_linearscale_ext_t * ext = lv_obj_get_ext_attr(lscale);

	lv_coord_t objw = lv_obj_get_width(lscale);
	lv_coord_t objh = lv_obj_get_height(lscale);
	bool hor = objw >= objh ? true : false;
	int16_t level = (int32_t)((int32_t)(ext->cur_value - ext->min_value) * ext->line_cnt) / (ext->max_value - ext->min_value);

	/*Calculate the indicator area*/
	lv_style_int_t bg_left = lv_obj_get_style_pad_left(lscale, LV_LINEARSCALE_PART_MAIN);
	lv_style_int_t bg_right = lv_obj_get_style_pad_right(lscale, LV_LINEARSCALE_PART_MAIN);
	lv_style_int_t bg_top = lv_obj_get_style_pad_top(lscale, LV_LINEARSCALE_PART_MAIN);
	lv_style_int_t bg_bottom = lv_obj_get_style_pad_bottom(lscale, LV_LINEARSCALE_PART_MAIN);

	/*Respect padding and minimum width/height too*/
	lv_area_t indic_area;
	lv_area_copy(&indic_area, &lscale->coords);
	indic_area.x1 += bg_left;
	indic_area.x2 -= bg_right;
	indic_area.y1 += bg_top;
	indic_area.y2 -= bg_bottom;

	lv_coord_t indicw = lv_area_get_width(&indic_area);
	lv_coord_t indich = lv_area_get_height(&indic_area);
	lv_style_int_t scale_width = lv_obj_get_style_scale_width(lscale, part);

	lv_draw_line_dsc_t line_dsc;
	lv_draw_line_dsc_init(&line_dsc);
	lv_obj_init_draw_line_dsc(lscale, part, &line_dsc);
	line_dsc.raw_end = 1;

	lv_color_t main_color = lv_obj_get_style_line_color(lscale, part);
	lv_color_t grad_color = lv_obj_get_style_scale_grad_color(lscale, part);
	lv_color_t end_color = lv_obj_get_style_scale_end_color(lscale, part);
	lv_style_int_t end_line_width = lv_obj_get_style_scale_end_line_width(lscale, part);

	lv_draw_label_dsc_t label_dsc;
	lv_draw_label_dsc_init(&label_dsc);
	lv_obj_init_draw_label_dsc(lscale, LV_LINEARSCALE_PART_MAIN, &label_dsc);

	int16_t label_counter = 0;
	if (ext->label_cnt > 1)
		label_counter = ext->line_cnt / (ext->label_cnt - 1);

	if (ext->line_cnt > 1)
	{
		uint16_t i;
		for (i = 0; i < ext->line_cnt; i++) {
			int minor = (i % label_counter) != 0;
			lv_style_int_t tick_width = minor ? scale_width / 2 : scale_width;
			lv_point_t p1, p2;
			if (hor) {
				int width = indic_area.x2 - indic_area.x1;
				int x = (100 * width * i / (ext->line_cnt - 1)) / 100;
				p1.x = indic_area.x1 + x;
				p2.x = p1.x;
				if (ext->align == LV_LINEARSCALE_ALIGN_TOP) {
					p1.y = indic_area.y1;
					p2.y = indic_area.y1 + tick_width;
				}
				else {
					p1.y = indic_area.y2 - tick_width;
					p2.y = indic_area.y2;
				}
			}
			else {
				int height = indic_area.y2 - indic_area.y1;
				int y = (100 * height * i / (ext->line_cnt - 1)) / 100;
				p1.y = indic_area.y2 - y;
				p2.y = p1.y;
				if (ext->align == LV_LINEARSCALE_ALIGN_LEFT) {
					p1.x = indic_area.x1;
					p2.x = indic_area.x1 + tick_width;
				}
				else {
					p1.x = indic_area.x2 - tick_width;
					p2.x = indic_area.x2;
				}
			}

			if (i >= level) {
				line_dsc.color = end_color;
				line_dsc.width = end_line_width;
			}
			else {
				line_dsc.color = lv_color_mix(grad_color, main_color, (255 * i) / ext->line_cnt);
			}

			lv_draw_line(&p1, &p2, clip_area, &line_dsc);

			if (!minor)
			{
				/*Get label text*/
				int32_t scale_act = (int32_t)((int32_t)(ext->max_value - ext->min_value) * i) / (ext->line_cnt - 1);
				scale_act += ext->min_value;
				char scale_txt[16];
				if (ext->format_cb == NULL)
					_lv_utils_num_to_str(scale_act, scale_txt);
				else
					ext->format_cb(lscale, scale_txt, sizeof(scale_txt), scale_act);

				lv_area_t label_cord;
				lv_point_t label_size;
				_lv_txt_get_size(&label_size, scale_txt, label_dsc.font, label_dsc.letter_space, label_dsc.line_space,
					LV_COORD_MAX, LV_TXT_FLAG_NONE);

				/*Draw the label*/
				if (hor) {
					if (ext->align == LV_LINEARSCALE_ALIGN_TOP) {
						label_cord.x1 = p1.x - label_size.x / 2;
						label_cord.y1 = p2.y + bg_bottom;
					}
					else {
						label_cord.x1 = p1.x - label_size.x / 2;
						label_cord.y1 = p1.y - tick_width - label_size.y / 8;
					}
					label_cord.x2 = label_cord.x1 + label_size.x;
					label_cord.y2 = label_cord.y1 + label_size.y;
				}
				else {
					if (ext->align == LV_LINEARSCALE_ALIGN_LEFT) {
						label_cord.x1 = p2.x;
						label_cord.y1 = p1.y - label_size.y / 2;
					}
					else {
						label_cord.x1 = p1.x - label_size.x;
						label_cord.y1 = p1.y - label_size.y / 2;
					}
					label_cord.x2 = label_cord.x1 + label_size.x;
					label_cord.y2 = label_cord.y1 + label_size.y;
				}

				lv_draw_label(&label_cord, clip_area, &label_dsc, scale_txt, NULL);
			}
		}
	}
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
#include <stdlib.h>

 /**
  * Handle the drawing related tasks of the line meters
  * @param lscale pointer to an object
  * @param clip_area the object will be drawn only in this area
  * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
  *                                  (return 'true' if yes)
  *             LV_DESIGN_DRAW: draw the object (always return 'true')
  *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
  * @param return an element of `lv_design_res_t`
  */
static lv_design_res_t lv_linearscale_design(lv_obj_t * lscale, const lv_area_t * clip_area, lv_design_mode_t mode)
{
	/*Return false if the object is not covers the mask_p area*/
	if (mode == LV_DESIGN_COVER_CHK) {
		return LV_DESIGN_RES_NOT_COVER;
	}
	/*Draw the object*/
	else if (mode == LV_DESIGN_DRAW_MAIN) {
		lv_draw_rect_dsc_t bg_dsc;
		lv_draw_rect_dsc_init(&bg_dsc);
		lv_obj_init_draw_rect_dsc(lscale, LV_LINEARSCALE_PART_MAIN, &bg_dsc);
		lv_draw_rect(&lscale->coords, clip_area, &bg_dsc);
		lv_linearscale_draw_scale(lscale, clip_area, LV_LINEARSCALE_PART_MAIN);
	}
	/*Post draw when the children are drawn*/
	else if (mode == LV_DESIGN_DRAW_POST) {
	}

	return LV_DESIGN_RES_OK;
}

/**
 * Get the style descriptor of a part of the object
 * @param lscale pointer the object
 * @param part the part from `lv_linearscale_part_t`. (LV_LINEARSCALE_PART_...)
 * @return pointer to the style descriptor of the specified part
 */
static lv_style_list_t * lv_linearscale_get_style(lv_obj_t * lscale, uint8_t part)
{
	LV_ASSERT_OBJ(lscale, LV_OBJX_NAME);

	lv_linearscale_ext_t * ext = lv_obj_get_ext_attr(lscale);
	lv_style_list_t * style_dsc_p;

	switch (part) {
	case LV_LINEARSCALE_PART_MAIN:
		style_dsc_p = &lscale->style_list;
		break;
	default:
		style_dsc_p = NULL;
	}

	return style_dsc_p;
}

/**
 * Signal function of the linear scale
 * @param lscale pointer to a linear scale object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_linearscale_signal(lv_obj_t * lscale, lv_signal_t sign, void * param)
{
	lv_res_t res;
	if (sign == LV_SIGNAL_GET_STYLE) {
		lv_get_style_info_t * info = param;
		info->result = lv_linearscale_get_style(lscale, info->part);
		if (info->result != NULL) return LV_RES_OK;
		else return ancestor_signal(lscale, sign, param);
	}

	/* Include the ancient signal function */
	res = ancestor_signal(lscale, sign, param);
	if (res != LV_RES_OK) return res;
	if (sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, LV_OBJX_NAME);

	if (sign == LV_SIGNAL_CLEANUP) {
		/*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
	}
	else if (sign == LV_SIGNAL_STYLE_CHG) {
		lv_obj_refresh_ext_draw_pad(lscale);
		lv_obj_invalidate(lscale);
	}

	return res;
}

#endif
