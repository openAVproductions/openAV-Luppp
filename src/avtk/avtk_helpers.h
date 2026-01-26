#ifndef AVTK_HELPERS_H
#define AVTK_HELPERS_H

#include <cairo.h>
#include <FL/Fl.H>
#include <FL/platform.H>
#include <FL/fl_draw.H>

#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#include <cairo-quartz.h>
#endif

// Helper to manage per-widget Cairo contexts
class AvtkCairoHelper {
private:
    cairo_surface_t* surface;
    cairo_t* cr;
    int x_pos, y_pos, width, height;
    
public:
    AvtkCairoHelper(int x, int y, int w, int h) : x_pos(x), y_pos(y), width(w), height(h) {
        // Always use image surface for drawing, then blit to screen
        // This avoids issues with fl_gc being NULL before window is shown
        surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width > 0 ? width : 1, height > 0 ? height : 1);
        cr = cairo_create(surface);
        
        // Translate Cairo coordinates so that (x, y) in widget space maps to (0, 0) in Cairo
        // This allows existing drawing code using absolute coordinates to work
        cairo_translate(cr, -x_pos, -y_pos);
    }
    
    ~AvtkCairoHelper() {
        // Draw the cairo surface to FLTK's current drawing context
        if (surface && width > 0 && height > 0) {
            cairo_surface_flush(surface);
            unsigned char* data = cairo_image_surface_get_data(surface);
            int stride = cairo_image_surface_get_stride(surface);
            
            // ARGB32 format from Cairo - convert to RGB for FLTK
            // Cairo uses BGRA on little-endian, ARGB on big-endian
            // For macOS (little-endian): B G R A in memory
            unsigned char* rgb_data = new unsigned char[width * height * 3];
            
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    int cairo_idx = i * stride + j * 4;
                    int rgb_idx = (i * width + j) * 3;
                    
                    // Cairo BGRA -> RGB
                    rgb_data[rgb_idx + 0] = data[cairo_idx + 2]; // R
                    rgb_data[rgb_idx + 1] = data[cairo_idx + 1]; // G
                    rgb_data[rgb_idx + 2] = data[cairo_idx + 0]; // B
                }
            }
            
            // Draw at widget position
            fl_draw_image(rgb_data, x_pos, y_pos, width, height, 3, 0);
            
            delete[] rgb_data;
        }
        
        if (cr) cairo_destroy(cr);
        if (surface) cairo_surface_destroy(surface);
    }
    
    cairo_t* context() { return cr; }
};

// Simple wrapper to replace Fl::cairo_cc()
#define avtk_cairo_begin(w, h) AvtkCairoHelper _cairo_helper(x, y, (w), (h)); cairo_t* cr = _cairo_helper.context()

#endif // AVTK_HELPERS_H
