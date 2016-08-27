#include <pebble.h>
#include "thob_q_ii.h"

static Window *window;
static Layer *s_simple_bg_layer, *s_date_layer, *s_hands_layer, *s_percent_layer;
static TextLayer *s_date_label, *s_week_label, *s_percent_label;

static GBitmap *dail_bitmap, *low_batt_bitmap, *charge_batt_bitmap;
static BitmapLayer *dail_layer, *bottom_border_layer, *low_batt_layer, *charge_batt_layer;

static GPath *s_tick_paths[NUM_CLOCK_TICKS]; //number of clock ticks no 3 + 9 and twice twelf
static GPath *s_second_arrow,*s_second_arrow_b,*s_second_arrow_90,*s_minute_arrow,*s_minute_arrow_90,*s_minute_arrow_b, *s_hour_arrow, *s_hour_arrow_b;
static char s_date_buffer[30]; //date text buffer
static char s_week_buffer[4]; //week text buffer
static char s_percent_buffer[5]; //battery percent buffer
static char s_debug_buffer[50];
static int dycenter=-12, batt_state=0;

bool bl_conf_sym_batt, bl_conf_sec_color; //show the batt symbol if true, use color if its true
int int_conf_week_type, int_conf_batt_lvl, int_conf_date_format, int_conf_date_deli, int_conf_sec_vis, int_conf_batt_perc;

int b_detector; //detects whether the tips should bold
  
GColor gcArrowFill, gcSecondFill, gcSecondStroke, gcArrowStroke, gcMiddleDot, gcDateText, gcWatchFaceBG, gcWatchFaceVG;

#define KEY_WEEK_TYPE 0
#define KEY_SYM_BATT 1
#define KEY_SEC_COL 2
#define KEY_BATT_LVL 3
#define KEY_DATE_FORMAT 4
#define KEY_DATE_DELI 5
#define KEY_SEC_VIS 6
#define KEY_BATT_PERC 7
  
#define KEY_WEEK_EU 0
#define KEY_WEEK_US 1
#define KEY_WEEK_OT 2
  
#define KEY_DATE_DMY 0
#define KEY_DATE_MDY 1
#define KEY_DATE_YMD 2
#define KEY_DATE_DMY2 3
#define KEY_DATE_MDY2 4
#define KEY_DATE_Y2MD 5
#define KEY_DATE_DM2Y 6
#define KEY_DATE_M2DY 7
#define KEY_DATE_YM2D 8
#define KEY_DATE_DM2Y2 9
#define KEY_DATE_M2DY2 10
#define KEY_DATE_Y2M2D 11
  
#define KEY_DELI_DDP 0
#define KEY_DELI_DDB 1
#define KEY_DELI_DDM 2
#define KEY_DELI_DDS 3

#define KEY_SEC_VIS_YES 0
#define KEY_SEC_VIS_NOS 1
#define KEY_SEC_VIS_NOH 2
  
static void hands_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
  int16_t second_hand_length = bounds.size.w / 2 - 27;
  
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  int32_t second_angle = TRIG_MAX_ANGLE * t->tm_sec / 60;
  
//  APP_LOG(APP_LOG_LEVEL_INFO, "batt_state: %d, %d",batt_state,bl_conf_sym_batt);

  // hour/minute hand
  graphics_context_set_fill_color(ctx, gcArrowFill);
  graphics_context_set_stroke_color(ctx, gcArrowStroke);
  
  //gpath_rotate_to(s_hour_arrow, (1 + TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6));
  
  //draw houre hand
  b_detector=(((uint32_t)(TRIG_MAX_ANGLE * (((t->tm_hour % 12)*60*60)+(t->tm_min*60)+t->tm_sec))/43200))%(TRIG_MAX_ANGLE/4);
  
  if(b_detector>=5461 && b_detector<10922){
    gpath_rotate_to(s_hour_arrow, 1+((uint32_t)(TRIG_MAX_ANGLE * (((t->tm_hour % 12)*60*60)+(t->tm_min*60)+t->tm_sec))/43200)); 
    gpath_draw_filled(ctx, s_hour_arrow);
    gpath_draw_outline(ctx, s_hour_arrow);
  }else{
    gpath_rotate_to(s_hour_arrow_b, 1+((uint32_t)(TRIG_MAX_ANGLE * (((t->tm_hour % 12)*60*60)+(t->tm_min*60)+t->tm_sec))/43200)); 
    gpath_draw_filled(ctx, s_hour_arrow_b);
    gpath_draw_outline(ctx, s_hour_arrow_b);
  }
  
  //draw minute hand
  b_detector=(TRIG_MAX_ANGLE * (t->tm_min*60+t->tm_sec) / 3600)%(TRIG_MAX_ANGLE/4);
  
  if(b_detector>=5461 && b_detector<10922){
    gpath_rotate_to(s_minute_arrow, 1 + TRIG_MAX_ANGLE * (t->tm_min*60+t->tm_sec) / 3600);
    gpath_rotate_to(s_minute_arrow_90, 2 + TRIG_MAX_ANGLE * (t->tm_min*60+t->tm_sec-900) / 3600);
    gpath_draw_filled(ctx, s_minute_arrow);
    gpath_draw_filled(ctx, s_minute_arrow_90);
    gpath_draw_outline(ctx, s_minute_arrow);
  }else{
    gpath_rotate_to(s_minute_arrow_b, 1 + TRIG_MAX_ANGLE * (t->tm_min*60+t->tm_sec) / 3600);
    gpath_rotate_to(s_minute_arrow_90, 2 + TRIG_MAX_ANGLE * (t->tm_min*60+t->tm_sec-900) / 3600);
    gpath_draw_filled(ctx, s_minute_arrow_b);
    gpath_draw_filled(ctx, s_minute_arrow_90);
    gpath_draw_outline(ctx, s_minute_arrow_b);
  }
  
  //draw second hand
  if(int_conf_sec_vis==KEY_SEC_VIS_YES){
    graphics_context_set_fill_color(ctx, gcSecondFill);
    graphics_context_set_stroke_color(ctx, gcSecondStroke);
    
    b_detector=(TRIG_MAX_ANGLE * t->tm_sec / 60)%(TRIG_MAX_ANGLE/4);
    
    if(b_detector>=5461 && b_detector<10922){
       gpath_rotate_to(s_second_arrow, 1+TRIG_MAX_ANGLE * t->tm_sec / 60);
       gpath_rotate_to(s_second_arrow_90, (TRIG_MAX_ANGLE * (t->tm_sec-15) / 60)+2);
    
       gpath_draw_filled(ctx, s_second_arrow);
       gpath_draw_filled(ctx, s_second_arrow_90);
       gpath_draw_outline(ctx, s_second_arrow);
    }else{
       gpath_rotate_to(s_second_arrow_b, 1+TRIG_MAX_ANGLE * t->tm_sec / 60);
       gpath_rotate_to(s_second_arrow_90, (TRIG_MAX_ANGLE * (t->tm_sec-15) / 60)+2);
    
       gpath_draw_filled(ctx, s_second_arrow_b);
       gpath_draw_filled(ctx, s_second_arrow_90);
       gpath_draw_outline(ctx, s_second_arrow_b);
    }
    
    graphics_context_set_fill_color(ctx, gcArrowFill);
    graphics_context_set_stroke_color(ctx, gcArrowStroke);
  } 
    // dot in the middle
    graphics_context_set_fill_color(ctx, gcMiddleDot);
    graphics_fill_rect(ctx, GRect(bounds.size.w / 2 - 1, bounds.size.h / 2 + dycenter - 1, 3, 3), 0, 2);
  
//Draw batt level in percent ------------------------------------------

  if(batt_state!=-1){
    snprintf(s_percent_buffer, sizeof(s_percent_buffer), "%i%%", batt_state);
  }else{
    snprintf(s_percent_buffer, sizeof(s_percent_buffer), "---");
  }
  text_layer_set_text(s_percent_label,s_percent_buffer);
}

static void date_update_proc(Layer *layer, GContext *ctx) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  char deli[]="./- ";
  char s_date_buffer2[30];

/*    switch(int_conf_date_deli){
    case KEY_DELI_DDP:
      
    break;
    case KEY_DELI_DDB:
      strftime(s_week_buffer, sizeof(s_week_buffer), "w%V", t);
    break;
    case KEY_DELI_DDM:
      strftime(s_week_buffer, sizeof(s_week_buffer), "w%V", t);
    break;
    }*/
  
    switch(int_conf_date_format){
      case KEY_DATE_DMY:
        strftime(s_date_buffer2, sizeof(s_date_buffer2), "%d%%c%b%%c%Y", t);
      break;
      case KEY_DATE_MDY:
        strftime(s_date_buffer2, sizeof(s_date_buffer2), "%b%%c%d%%c%Y", t);
      break;
      case KEY_DATE_YMD:
        strftime(s_date_buffer2, sizeof(s_date_buffer2), "%Y%%c%b%%c%d", t);
      break;
      case KEY_DATE_DMY2:
        strftime(s_date_buffer2, sizeof(s_date_buffer2), "%d%%c%b%%c%y", t);
      break;
      case KEY_DATE_MDY2:
        strftime(s_date_buffer2, sizeof(s_date_buffer2), "%b%%c%d%%c%y", t);
      break;
      case KEY_DATE_Y2MD:
        strftime(s_date_buffer2, sizeof(s_date_buffer2), "%y%%c%b%%c%d", t);
      break;
      
      case KEY_DATE_DM2Y:
        strftime(s_date_buffer2, sizeof(s_date_buffer2), "%d%%c%m%%c%Y", t);
      break;
      case KEY_DATE_M2DY:
        strftime(s_date_buffer2, sizeof(s_date_buffer2), "%m%%c%d%%c%Y", t);
      break;
      case KEY_DATE_YM2D:
        strftime(s_date_buffer2, sizeof(s_date_buffer2), "%Y%%c%m%%c%d", t);
      break;
      case KEY_DATE_DM2Y2:
        strftime(s_date_buffer2, sizeof(s_date_buffer2), "%d%%c%m%%c%y", t);
      break;
      case KEY_DATE_M2DY2:
        strftime(s_date_buffer2, sizeof(s_date_buffer2), "%m%%c%d%%c%y", t);
      break;
      case KEY_DATE_Y2M2D:
        strftime(s_date_buffer2, sizeof(s_date_buffer2), "%y%%c%m%%c%d", t);
      break;
  }
  
  snprintf(s_date_buffer, sizeof(s_date_buffer), s_date_buffer2, deli[int_conf_date_deli], deli[int_conf_date_deli]);
  
//  APP_LOG(APP_LOG_LEVEL_INFO, "date format: %c, %d",deli[int_conf_date_deli],int_conf_date_deli);
  
//  strftime(s_date_buffer, sizeof(s_date_buffer), "%d.%b.%Y", t);
  
  text_layer_set_text(s_date_label,s_date_buffer);
  
  switch(int_conf_week_type){
    case KEY_WEEK_EU:
      strftime(s_week_buffer, sizeof(s_week_buffer), "w%V", t);
    break;
    case KEY_WEEK_US:
      strftime(s_week_buffer, sizeof(s_week_buffer), "w%U", t);
    break;
    case KEY_WEEK_OT:
      strftime(s_week_buffer, sizeof(s_week_buffer), "w%W", t);
    break;
  }

  text_layer_set_text(s_week_label,s_week_buffer);
}

//-- get battery state --------------------------------------------------
static int get_batt_state(){
  int batt_state;
  BatteryChargeState charge_state = battery_state_service_peek();
  if (charge_state.is_charging) {
    batt_state=-1;
  } else {
    batt_state=charge_state.charge_percent;
  }
  return batt_state;
}

static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(window_get_root_layer(window));
}

static void options_update(){
//hidde unhidde Layer for battery symbole dep. on options ---------------
  if(bl_conf_sym_batt==true){
    if (batt_state<0) {
      layer_set_hidden(bitmap_layer_get_layer(charge_batt_layer), false);
    } else {
        layer_set_hidden(bitmap_layer_get_layer(charge_batt_layer), true);
        if (batt_state<=int_conf_batt_lvl){
          layer_set_hidden(bitmap_layer_get_layer(low_batt_layer), false);
        }else{
          layer_set_hidden(bitmap_layer_get_layer(low_batt_layer), true);
        }
    }  
  }else{
    layer_set_hidden(bitmap_layer_get_layer(charge_batt_layer), true);
    layer_set_hidden(bitmap_layer_get_layer(low_batt_layer), true);
  }
  
  if(int_conf_batt_perc==true){
    layer_set_hidden(text_layer_get_layer(s_percent_label), false);
  }else{
    layer_set_hidden(text_layer_get_layer(s_percent_label), true);
  }
  
  if((int_conf_sec_vis==KEY_SEC_VIS_YES) || (int_conf_sec_vis==KEY_SEC_VIS_NOS)){
    tick_timer_service_subscribe(SECOND_UNIT, handle_tick);
  }else{
    tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
  }

  //set second tip color dependig on options -------------------------------
  #ifdef PBL_COLOR
    if(bl_conf_sec_color==true){
      gcSecondFill=GColorRed;// GColorDarkCandyAppleRed;
    }else{
      gcSecondFill=GColorBlack;// GColorDarkCandyAppleRed;
    }
    gcSecondStroke=GColorWhite;//GColorMelon;//GColorRed;
  #else
    if(bl_conf_sec_color==true){
      gcSecondFill=GColorBlack;
    }else{
      gcSecondFill=GColorBlack;
    }
    gcSecondStroke=GColorWhite;
  #endif
  }

static void handle_battery(BatteryChargeState charge_state) {
//  APP_LOG(APP_LOG_LEVEL_INFO, "in handle_battery");
  batt_state=get_batt_state();
  options_update();
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window); //get root Layer of main Window
  GRect bounds = layer_get_bounds(window_layer); // Get the bounds of the window for sizing something
  
  //Check for saved option
  if(persist_exists(KEY_SYM_BATT)){
    bl_conf_sym_batt = persist_read_bool(KEY_SYM_BATT);
  }else
     {bl_conf_sym_batt=true;}
  
  if(persist_exists(KEY_SEC_COL)){
    bl_conf_sec_color = persist_read_bool(KEY_SEC_COL);
  }else
     {bl_conf_sec_color=true;}
  
  if(persist_exists(KEY_WEEK_TYPE)){
    int_conf_week_type = persist_read_int(KEY_WEEK_TYPE);
  }else
     {int_conf_week_type=KEY_WEEK_EU;}
  
  if(persist_exists(KEY_DATE_FORMAT)){
    int_conf_date_format = persist_read_int(KEY_DATE_FORMAT);
  }else
     {int_conf_date_format=KEY_DATE_DMY;}
  
  if(persist_exists(KEY_DATE_DELI)){
    int_conf_date_deli = persist_read_int(KEY_DATE_DELI);
  }else
     {int_conf_date_deli=KEY_DELI_DDP;}
  
  if(persist_exists(KEY_BATT_LVL)){
    int_conf_batt_lvl = persist_read_int(KEY_BATT_LVL);
  }else
     {int_conf_batt_lvl=10;}
  
  if(persist_exists(KEY_SEC_VIS)){
    int_conf_sec_vis = persist_read_int(KEY_SEC_VIS);
  }else
     {int_conf_sec_vis=KEY_SEC_VIS_YES;}
  
  if(persist_exists(KEY_BATT_PERC)){
    int_conf_batt_perc = persist_read_int(KEY_BATT_PERC);
  }else
     {int_conf_batt_perc=false;}
  
  APP_LOG(APP_LOG_LEVEL_INFO, "KEY_SYM_BATT: %d",bl_conf_sym_batt);
  APP_LOG(APP_LOG_LEVEL_INFO, "KEY_SEC_COL: %d",bl_conf_sec_color);
  APP_LOG(APP_LOG_LEVEL_INFO, "KEY_WEEK_TYPE: %d",int_conf_week_type);
  APP_LOG(APP_LOG_LEVEL_INFO, "KEY_BATT_LVL: %d",int_conf_batt_lvl);
  
  APP_LOG(APP_LOG_LEVEL_INFO, "KEY_DATE_FORMAT: %d",int_conf_date_format);
  APP_LOG(APP_LOG_LEVEL_INFO, "KEY_DATE_DELI: %d",int_conf_date_deli);
  
  GPoint center = grect_center_point(&bounds);
  batt_state=get_batt_state();
  
//-- Set Image Layer for Dail -------------------------------------------  
  dail_bitmap = gbitmap_create_with_resource(RESOURCE_ID_DAIL3);
  
  
  
  GSize image_size = gbitmap_get_bounds(dail_bitmap).size;
  
#ifdef PBL_PLATFORM_BASALT
//  GSize image_size = gbitmap_get_bounds(dail_bitmap).size;
#else
//  GSize image_size = dail_bitmap->bounds.size;
#endif
  GRect image_frame = GRect(center.x, center.y+dycenter, image_size.w, image_size.h);
  image_frame.origin.x -= (image_size.w / 2);
  image_frame.origin.y -= (image_size.h / 2);
  
  dail_layer = bitmap_layer_create(image_frame);
  bitmap_layer_set_bitmap(dail_layer, dail_bitmap);
  
  layer_add_child(window_layer, bitmap_layer_get_layer(dail_layer));

//-----------------------------------------------------------------------

  bottom_border_layer = bitmap_layer_create(GRect(0, bounds.size.h-22, bounds.size.w, 22));
  bitmap_layer_set_background_color(bottom_border_layer, GColorWhite);
  layer_add_child(window_layer, bitmap_layer_get_layer(bottom_border_layer));
  
//-- Set Image Layer for Symbols -------------------------------------------  
  low_batt_bitmap = gbitmap_create_with_resource(RESOURCE_ID_LOW_BATT);
  charge_batt_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CHARGE_BATT);
  
  image_frame = GRect(center.x+1, center.y+30, 16, 16);
  image_frame.origin.x -= (16 / 2);
  image_frame.origin.y -= (16 / 2);
  
  low_batt_layer = bitmap_layer_create(image_frame);
  charge_batt_layer = bitmap_layer_create(image_frame);
  bitmap_layer_set_bitmap(low_batt_layer, low_batt_bitmap);
  bitmap_layer_set_bitmap(charge_batt_layer, charge_batt_bitmap);
  
  layer_add_child(window_layer, bitmap_layer_get_layer(low_batt_layer));
  layer_set_hidden(bitmap_layer_get_layer(low_batt_layer), true);
  
  layer_add_child(window_layer, bitmap_layer_get_layer(charge_batt_layer));
  layer_set_hidden(bitmap_layer_get_layer(charge_batt_layer), true);

//-----------------------------------------------------------------------
  
  s_hands_layer = layer_create(bounds);
  layer_set_update_proc(s_hands_layer, hands_update_proc);
  layer_add_child(window_layer, s_hands_layer);
  
  s_date_layer = layer_create(bounds);
  layer_set_update_proc(s_date_layer, date_update_proc);
  layer_add_child(window_layer, s_date_layer);

  s_date_label = text_layer_create(GRect(2, bounds.size.h-28, bounds.size.w, 28));
  text_layer_set_text(s_date_label, s_date_buffer);
  text_layer_set_text_alignment(s_date_label, GTextAlignmentLeft);
  text_layer_set_background_color(s_date_label, GColorClear);
  text_layer_set_text_color(s_date_label, gcDateText);
  text_layer_set_font(s_date_label, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(s_date_layer, text_layer_get_layer(s_date_label));
  
  s_week_label = text_layer_create(GRect(0, bounds.size.h-28, bounds.size.w-2, 28));
  text_layer_set_text(s_week_label, s_week_buffer);
  text_layer_set_text_alignment(s_week_label, GTextAlignmentRight);
  text_layer_set_background_color(s_week_label, GColorClear);
  text_layer_set_text_color(s_week_label, gcDateText);
  text_layer_set_font(s_week_label, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  //Roboto 21 Condensed
  layer_add_child(s_date_layer, text_layer_get_layer(s_week_label));

//create batt percent layer -------------------------------------------------------
  s_percent_layer = layer_create(bounds);
  
  layer_set_update_proc(s_percent_layer, date_update_proc);
  layer_add_child(window_layer, s_percent_layer);
  
  s_percent_label = text_layer_create(GRect(bounds.size.w-31, -0, 30, 14));
  text_layer_set_text_alignment(s_percent_label, GTextAlignmentRight);
  text_layer_set_background_color(s_percent_label, GColorClear);
  
  text_layer_set_font(s_percent_label, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  layer_add_child(s_percent_layer, text_layer_get_layer(s_percent_label));
  
  batt_state=get_batt_state();
  options_update();
}

static void window_unload(Window *window) {
//  layer_destroy(s_simple_bg_layer);
  text_layer_destroy(s_week_label);
  text_layer_destroy(s_date_label);
  
  layer_destroy(s_date_layer);
  layer_destroy(s_hands_layer);

  bitmap_layer_destroy(charge_batt_layer);
  bitmap_layer_destroy(low_batt_layer);
  
  gbitmap_destroy(charge_batt_bitmap);
  gbitmap_destroy(low_batt_bitmap);

  bitmap_layer_destroy(bottom_border_layer);
  bitmap_layer_destroy(dail_layer);
  
  gbitmap_destroy(dail_bitmap);
}

//create the AppMessageInboxReceived handler that will process any received messages
static void in_recv_handler(DictionaryIterator *iterator, void *context){
  //Get Tuple
  Tuple *t = dict_read_first(iterator);
  APP_LOG(APP_LOG_LEVEL_INFO, "inTuple");

  // Process all pairs present
  while(t != NULL) {
    // Process this pair's key
    switch (t->key) {
      case KEY_BATT_LVL:
      APP_LOG(APP_LOG_LEVEL_INFO, "KEY_BATT_LVL: %s", t->value->cstring);
            if(strcmp(t->value->cstring, "10") == 0){
              persist_write_int(KEY_BATT_LVL, 10);
              int_conf_batt_lvl=10;
            }
            else if(strcmp(t->value->cstring, "20") == 0){
              persist_write_int(KEY_BATT_LVL, 20);
              int_conf_batt_lvl=20;
            }
            else if(strcmp(t->value->cstring, "30") == 0){
              persist_write_int(KEY_BATT_LVL, 30);
              int_conf_batt_lvl=30;
            }
      break;
      
      case KEY_WEEK_TYPE:
      APP_LOG(APP_LOG_LEVEL_INFO, "KEY_WEEK_TYPE: %s", t->value->cstring);
            if(strcmp(t->value->cstring, "weu") == 0){
              persist_write_int(KEY_WEEK_TYPE, KEY_WEEK_EU);
              int_conf_week_type=KEY_WEEK_EU;
            }
            else if(strcmp(t->value->cstring, "wus") == 0){
              persist_write_int(KEY_WEEK_TYPE, KEY_WEEK_US);
              int_conf_week_type=KEY_WEEK_US;
            }
            else if(strcmp(t->value->cstring, "wot") == 0){
              persist_write_int(KEY_WEEK_TYPE, KEY_WEEK_OT);
              int_conf_week_type=KEY_WEEK_OT;
            }
        break;
      
      case KEY_DATE_FORMAT:
        APP_LOG(APP_LOG_LEVEL_INFO, "KEY_DATE_FORMAT: %s", t->value->cstring);
            if(strcmp(t->value->cstring, "dmy") == 0){
              persist_write_int(KEY_DATE_FORMAT, KEY_DATE_DMY);
              int_conf_date_format=KEY_DATE_DMY;
            }
            else if(strcmp(t->value->cstring, "mdy") == 0){
              persist_write_int(KEY_DATE_FORMAT, KEY_DATE_MDY);
              int_conf_date_format=KEY_DATE_MDY;
            }
            else if(strcmp(t->value->cstring, "ymd") == 0){
              persist_write_int(KEY_DATE_FORMAT, KEY_DATE_YMD);
              int_conf_date_format=KEY_DATE_YMD;
            }
            else if(strcmp(t->value->cstring, "dmy2") == 0){
              persist_write_int(KEY_DATE_FORMAT, KEY_DATE_DMY2);
              int_conf_date_format=KEY_DATE_DMY2;
            }
            else if(strcmp(t->value->cstring, "mdy2") == 0){
              persist_write_int(KEY_DATE_FORMAT, KEY_DATE_MDY2);
              int_conf_date_format=KEY_DATE_MDY2;
            }
            else if(strcmp(t->value->cstring, "y2md") == 0){
              persist_write_int(KEY_DATE_FORMAT, KEY_DATE_Y2MD);
              int_conf_date_format=KEY_DATE_Y2MD;
            }
      
                  if(strcmp(t->value->cstring, "dm2y") == 0){
              persist_write_int(KEY_DATE_FORMAT, KEY_DATE_DM2Y);
              int_conf_date_format=KEY_DATE_DM2Y;
            }
            else if(strcmp(t->value->cstring, "m2dy") == 0){
              persist_write_int(KEY_DATE_FORMAT, KEY_DATE_M2DY);
              int_conf_date_format=KEY_DATE_M2DY;
            }
            else if(strcmp(t->value->cstring, "ym2d") == 0){
              persist_write_int(KEY_DATE_FORMAT, KEY_DATE_YM2D);
              int_conf_date_format=KEY_DATE_YM2D;
            }
            else if(strcmp(t->value->cstring, "dm2y2") == 0){
              persist_write_int(KEY_DATE_FORMAT, KEY_DATE_DM2Y2);
              int_conf_date_format=KEY_DATE_DM2Y2;
            }
            else if(strcmp(t->value->cstring, "m2dy2") == 0){
              persist_write_int(KEY_DATE_FORMAT, KEY_DATE_M2DY2);
              int_conf_date_format=KEY_DATE_M2DY2;
            }
            else if(strcmp(t->value->cstring, "y2m2d") == 0){
              persist_write_int(KEY_DATE_FORMAT, KEY_DATE_Y2M2D);
              int_conf_date_format=KEY_DATE_Y2M2D;
            }
        break;
      
        case KEY_DATE_DELI:
      APP_LOG(APP_LOG_LEVEL_INFO, "KEY_DATE_DELI: %s", t->value->cstring);
            if(strcmp(t->value->cstring, "ddp") == 0){
              persist_write_int(KEY_DATE_DELI, KEY_DELI_DDP);
              int_conf_date_deli=KEY_DELI_DDP;
            }
            else if(strcmp(t->value->cstring, "ddb") == 0){
              persist_write_int(KEY_DATE_DELI, KEY_DELI_DDB);
              int_conf_date_deli=KEY_DELI_DDB;
            }
            else if(strcmp(t->value->cstring, "ddm") == 0){
              persist_write_int(KEY_DATE_DELI, KEY_DELI_DDM);
              int_conf_date_deli=KEY_DELI_DDM;
            }
            else if(strcmp(t->value->cstring, "dds") == 0){
              persist_write_int(KEY_DATE_DELI, KEY_DELI_DDS);
              int_conf_date_deli=KEY_DELI_DDS;
            }
        break;
      
      case KEY_SYM_BATT:
        APP_LOG(APP_LOG_LEVEL_INFO, "KEY_SYM_BATT: %s", t->value->cstring);
            if(strcmp(t->value->cstring, "yes") == 0){
              persist_write_bool(KEY_SYM_BATT, true);
              bl_conf_sym_batt=true;
            }
            else if(strcmp(t->value->cstring, "no") == 0){
              persist_write_bool(KEY_SYM_BATT, false);
              bl_conf_sym_batt=false;
            }
        break;
      
      case KEY_SEC_COL:
        APP_LOG(APP_LOG_LEVEL_INFO, "KEY_SEC_COL: %s", t->value->cstring);
            if(strcmp(t->value->cstring, "sc_red") == 0){
              persist_write_bool(KEY_SEC_COL, true);
              bl_conf_sec_color=true;
            }
            else if(strcmp(t->value->cstring, "sc_black") == 0){
              persist_write_bool(KEY_SEC_COL, false);
              bl_conf_sec_color=false;
            }
        break;
      
      case KEY_SEC_VIS:
          APP_LOG(APP_LOG_LEVEL_INFO, "KEY_SEC_VIS: %s", t->value->cstring);
            if(strcmp(t->value->cstring, "yes") == 0){
              persist_write_int(KEY_SEC_VIS, KEY_SEC_VIS_YES);
              int_conf_sec_vis=KEY_SEC_VIS_YES;
            }
            else if(strcmp(t->value->cstring, "nosoft") == 0){
              persist_write_int(KEY_SEC_VIS, KEY_SEC_VIS_NOS);
              int_conf_sec_vis=KEY_SEC_VIS_NOS;
            }
            else if(strcmp(t->value->cstring, "nohard") == 0){
              persist_write_int(KEY_SEC_VIS, KEY_SEC_VIS_NOH);
              int_conf_sec_vis=KEY_SEC_VIS_NOH;
            }
        break;
      
      case KEY_BATT_PERC:
          APP_LOG(APP_LOG_LEVEL_INFO, "KEY_BATT_PERC: %s", t->value->cstring);
            if(strcmp(t->value->cstring, "yes") == 0){
                persist_write_int(KEY_BATT_PERC, true);
                int_conf_batt_perc=true;
            }else{
                persist_write_int(KEY_BATT_PERC, false);
                int_conf_batt_perc=false;}
      break;
    }
    // Get next pair, if any
    t = dict_read_next(iterator);
  }
  options_update();
}

static void init() {
  setlocale(LC_ALL, ""); //Set Localisation to system
  
  batt_state=get_batt_state(); //get battery state when app starts
  
  gcArrowFill=GColorBlack;
  
  gcArrowStroke=GColorWhite;
  gcMiddleDot=GColorWhite;
  gcDateText=GColorBlack;
  gcWatchFaceBG=GColorWhite;
  gcWatchFaceVG=GColorBlack;
  
  window = window_create(); //Creates a new Window on the heap and initalizes it with the default values

  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  
  window_stack_push(window, true); //push window on stack, true = animate
  
  //APP_LOG(APP_LOG_LEVEL_INFO, "window_stack_push");
  
  //open AppMessage to enable communication with the phone
  app_message_register_inbox_received((AppMessageInboxReceived) in_recv_handler);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  // init text buffer -------------
  s_date_buffer[0] = '\0';
  s_week_buffer[0] = '\0';

  // init hand paths
  s_second_arrow = gpath_create(&SECOND_HAND_POINTS);
  s_second_arrow_b = gpath_create(&SECOND_HAND_POINTS_B);
  s_second_arrow_90 = gpath_create(&SECOND_HAND_POINTS_90);
  s_minute_arrow = gpath_create(&MINUTE_HAND_POINTS); //Creates a new GPath on the heap based on a series of points described by a GPathInfo
  s_minute_arrow_b = gpath_create(&MINUTE_HAND_POINTS_B);
  s_minute_arrow_90 = gpath_create(&MINUTE_HAND_POINTS_90);
  s_hour_arrow = gpath_create(&HOUR_HAND_POINTS);
  s_hour_arrow_b = gpath_create(&HOUR_HAND_POINTS_B);
  
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  GPoint center = grect_center_point(&bounds);
  center.y=center.y+dycenter;
  gpath_move_to(s_second_arrow, center);
  gpath_move_to(s_second_arrow_b, center);
  gpath_move_to(s_second_arrow_90, center);
  gpath_move_to(s_minute_arrow, center);
  gpath_move_to(s_minute_arrow_b, center);
  gpath_move_to(s_minute_arrow_90, center);
  gpath_move_to(s_hour_arrow, center);
  gpath_move_to(s_hour_arrow_b, center);

/*  for (int i = 0; i < NUM_CLOCK_TICKS; ++i) {
    s_tick_paths[i] = gpath_create(&ANALOG_BG_POINTS[i]);
  }*/

  tick_timer_service_subscribe(SECOND_UNIT, handle_tick);
  battery_state_service_subscribe(handle_battery);
}

static void deinit() {
  gpath_destroy(s_second_arrow);
  gpath_destroy(s_second_arrow_b);
  gpath_destroy(s_second_arrow_90);
  gpath_destroy(s_minute_arrow);
  gpath_destroy(s_minute_arrow_b);
  gpath_destroy(s_minute_arrow_90);
  gpath_destroy(s_hour_arrow);
  gpath_destroy(s_hour_arrow_b);

/*  for (int i = 0; i < NUM_CLOCK_TICKS; ++i) {
    gpath_destroy(s_tick_paths[i]);
  }*/
  
  battery_state_service_unsubscribe();
  tick_timer_service_unsubscribe();
  window_destroy(window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}