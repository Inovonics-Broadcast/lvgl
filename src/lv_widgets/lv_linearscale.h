/**
 * @file lv_linearscale.h
 *
 */

#ifndef LV_LINEARSCALE_H
#define LV_LINEARSCALE_H

#ifdef __cplusplus
extern "C" {
#endif

	/*********************
	 *      INCLUDES
	 *********************/
#include "../lv_conf_internal.h"

#if LV_USE_LINEARSCALE != 0

#include "../lv_core/lv_obj.h"

	 /*********************
	  *      DEFINES
	  *********************/

	enum {
		LV_LINEARSCALE_ALIGN_TOP,
		LV_LINEARSCALE_ALIGN_BOT,
		LV_LINEARSCALE_ALIGN_LEFT,
		LV_LINEARSCALE_ALIGN_RIGHT,
	};

	typedef uint8_t lv_linearscale_align_t;

	typedef void(*lv_linearscale_format_cb_t)(lv_obj_t * gauge, char buf[], int bufsize, int32_t value);

	/**********************
	 *      TYPEDEFS
	 **********************/
	 /*Data of linear scale*/
	typedef struct {
		/*No inherited ext.*/ /*Ext. of ancestor*/
		/*New data for this type */
		uint16_t line_cnt;     /*Count of lines */
		uint16_t label_cnt;
		int32_t cur_value;
		int32_t min_value;
		int32_t max_value;
		lv_linearscale_align_t align;
		lv_linearscale_format_cb_t format_cb;
	} lv_linearscale_ext_t;

	/*Styles*/
	enum {
		LV_LINEARSCALE_PART_MAIN = LV_OBJ_PART_MAIN,
		_LV_LINEARSCALE_PART_VIRTUAL_LAST,
		_LV_LINEARSCALE_PART_REAL_LAST = _LV_OBJ_PART_REAL_LAST,
	};
	typedef uint8_t lv_linearscale_part_t;

	/**********************
	 * GLOBAL PROTOTYPES
	 **********************/

	 /**
	  * Create a linear scale object
	  * @param par pointer to an object, it will be the parent of the new linear scale
	  * @param copy pointer to a line meter object, if not NULL then the new object will be copied from
	  * it
	  * @return pointer to the created line meter
	  */
	lv_obj_t * lv_linearscale_create(lv_obj_t * par, const lv_obj_t * copy);

	/*=====================
	 * Setter functions
	 *====================*/

	 /**
	  * Set a new value on the linear scale
	  * @param lscale pointer to a linear scale object
	  * @param value new value
	  */
	void lv_linearscale_set_value(lv_obj_t * lscale, int32_t value);

	/**
	 * Set minimum and the maximum values of a linear scale
	 * @param lscale pointer to the linear scale object
	 * @param min minimum value
	 * @param max maximum value
	 */
	void lv_linearscale_set_range(lv_obj_t * lscale, int32_t min, int32_t max);

	/**
	 * Set the number of lines for a linear scale
	 * @param lscale pointer to a linear scale object
	 * @param line_cnt number of lines
	 * @param label_cnt number of labels
	 */
	void lv_linearscale_set_scale(lv_obj_t * lscale, uint16_t line_cnt, uint16_t label_cnt);

	/**
	 * Set the alignment side for a linear scale
	 * @param lscale pointer to a linear scale object
	 * @param dir alignment direction
	 */
	void lv_linearscale_set_alignment(lv_obj_t * lscale, lv_linearscale_align_t dir);

	/**
	 * Assign a function to format scale values
	 * @param lscale pointer to a linear scale object
	 * @param format_cb pointer to function of lv_linearscale_format_cb_t
	 */
	void lv_linearscale_set_formatter_cb(lv_obj_t * lscale, lv_linearscale_format_cb_t format_cb);

	/*=====================
	 * Getter functions
	 *====================*/

	 /**
	  * Get the value of a linear scale
	  * @param lscale pointer to a linear scale object
	  * @return the value of the line meter
	  */
	int32_t lv_linearscale_get_value(const lv_obj_t * lscale);

	/**
	 * Get the minimum value of a line meter
	 * @param lscale pointer to a line meter object
	 * @return the minimum value of the line meter
	 */
	int32_t lv_linearscale_get_min_value(const lv_obj_t * lscale);

	/**
	 * Get the maximum value of a linear scale
	 * @param lscale pointer to a linear scale object
	 * @return the maximum value of the linear scale
	 */
	int32_t lv_linearscale_get_max_value(const lv_obj_t * lscale);

	/**
	 * Get the scale number of a linear scale
	 * @param lscale pointer to a linear scale object
	 * @return number of lines
	 */
	uint16_t lv_linearscale_get_line_count(const lv_obj_t * lscale);

	/**
	 * Get the scale number of a linear scale
	 * @param lscale pointer to a linear scale object
	 * @return number of labels
	 */
	uint16_t lv_linearscale_get_label_count(const lv_obj_t * lscale);

	void lv_linearscale_draw_scale(lv_obj_t * lscale, const lv_area_t * clip_area, uint8_t part);

	/**********************
	 *      MACROS
	 **********************/

#endif /*LV_USE_LINEARSCALE*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_LINEARSCALE_H*/