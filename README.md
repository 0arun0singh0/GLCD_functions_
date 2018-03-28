# GLCD_functions_
Functions Included:  

void enable_pulse();
void glcd_on();
void set_start_line(unsigned char);
void goto_col(unsigned char);
void goto_row(unsigned char);
void goto_xy(unsigned char, unsigned char);
void glcd_write(unsigned char);
unsigned char glcd_read(unsigned char);
void glcd_clear_line(unsigned char);
void glcd_clear();
void glcd_draw_pixel(unsigned char, unsigned char, unsigned char);
void glcd_text(unsigned char);
void glcd_num(unsigned int);
void glcd_string(unsigned char*);
void glcd_line(unsigned char, unsigned char, unsigned char, unsigned char, unsigned char);
void glcd_rect(unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char);
void glcd_circle(int, int, int, int, int);
