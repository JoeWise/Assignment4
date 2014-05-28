// $Id: interp.cpp,v 1.14 2014-05-08 18:32:56-07 - - $

#include <map>
#include <memory>
#include <string>
#include <vector>
using namespace std;

#include <GL/freeglut.h>

#include "debug.h"
#include "interp.h"
#include "shape.h"
#include "util.h"

map<string,interpreter::interpreterfn> interpreter::interp_map {
   {"define" , &interpreter::do_define },
   {"draw"   , &interpreter::do_draw   },
};

map<string,interpreter::factoryfn> interpreter::factory_map {
   {"text"     , &interpreter::make_text     },
   {"ellipse"  , &interpreter::make_ellipse  },
   {"circle"   , &interpreter::make_circle   },
   {"polygon"  , &interpreter::make_polygon  },
   {"rectangle", &interpreter::make_rectangle},
   {"square"   , &interpreter::make_square   },
   {"diamond"  , &interpreter::make_diamond  },
   {"triangle" , &interpreter::make_triangle },
   {"right_triangle" , &interpreter::make_right_triangle },
   {"isosceles", &interpreter::make_isosceles},
   {"equilateral", &interpreter::make_equilateral},
};

static unordered_map<string,void*> fontcode {
   {"Fixed-8x13"    , GLUT_BITMAP_8_BY_13       },
   {"Fixed-9x15"    , GLUT_BITMAP_9_BY_15       },
   {"Helvetica-10"  , GLUT_BITMAP_HELVETICA_10  },
   {"Helvetica-12"  , GLUT_BITMAP_HELVETICA_12  },
   {"Helvetica-18"  , GLUT_BITMAP_HELVETICA_18  },
   {"Times-Roman-10", GLUT_BITMAP_TIMES_ROMAN_10},
   {"Times-Roman-24", GLUT_BITMAP_TIMES_ROMAN_24},
};


interpreter::~interpreter() {
   for (const auto& itor: objmap) {
      cout << "objmap[" << itor.first << "] = "
           << *itor.second << endl;
   }
}

void interpreter::interpret (const parameters& params) {
   DEBUGF ('i', params);
   param begin = params.cbegin();
   string command = *begin;
   auto itor = interp_map.find (command);
   if (itor == interp_map.end()) throw runtime_error ("syntax error");
   interpreterfn func = itor->second;
   (this->*func) (++begin, params.cend());
}

void interpreter::do_define (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string name = *begin;
   objmap.insert ({name, make_shape (++begin, end)});
}


void interpreter::do_draw (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if (end - begin != 4) throw runtime_error ("syntax error");
   string name (begin[1]);
   shape_map::const_iterator itor = objmap.find (name);
   if (itor == objmap.end()) {
      throw runtime_error (name + ": no such shape");
   }
   vertex where {from_string<GLfloat> (begin[2]),
                 from_string<GLfloat> (begin[3])};
   rgbcolor color (begin[0]);
   object new_shape(itor->second, where, color);
   window::push_back(new_shape);
}

shape_ptr interpreter::make_shape (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string type = *begin++;
   auto itor = factory_map.find(type);
   if (itor == factory_map.end()) {
      throw runtime_error (type + ": no such shape");
   }
   factoryfn func = itor->second;
   return (this->*func) (begin, end);
}

shape_ptr interpreter::make_text (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   auto iter = begin;
   void* font_to_use = fontcode.at(*iter++);
   string txt = *iter++;
   for(; iter != end; ++iter){
      txt.append(" " + *iter);
   }
   return make_shared<text> (font_to_use, txt);
}

shape_ptr interpreter::make_ellipse (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   auto iter = begin;
   float width = stof(*iter++);
   float height = stof(*iter);
   return make_shared<ellipse> (GLfloat(width), GLfloat(height));
}

shape_ptr interpreter::make_circle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   return make_shared<circle> (GLfloat(stof(*begin)));
}

shape_ptr interpreter::make_polygon (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   vertex_list vertices;

   for (auto iter = begin; iter != end; ++iter){
      vertex v;
         v.xpos = stof(*iter++);
         v.ypos = stof(*iter);
         vertices.push_back(v);
   }

   return make_shared<polygon> (vertices);
}

shape_ptr interpreter::make_rectangle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   auto iter = begin;
   float width = stof(*iter++);
   float height = stof(*iter);
   return make_shared<rectangle> (GLfloat(width), GLfloat(height));
}

shape_ptr interpreter::make_square (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   return make_shared<square> (GLfloat(stof(*begin)));
}

shape_ptr interpreter::make_diamond (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   auto iter = begin;
   float width = stof(*iter++);
   float height = stof(*iter);
   return make_shared<diamond> (GLfloat(width), GLfloat(height));
}

shape_ptr interpreter::make_triangle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   vertex_list vertices;

   for (auto iter = begin; iter != end; ++iter){
      vertex v;
         v.xpos = stof(*iter++);
         v.ypos = stof(*iter);
         vertices.push_back(v);
   }

   return make_shared<triangle> (vertices);
}

shape_ptr interpreter::make_right_triangle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   auto iter = begin;
   float width = stof(*iter++);
   float height = stof(*iter);
   return make_shared<right_triangle>
          (GLfloat(width), GLfloat(height));
}

shape_ptr interpreter::make_isosceles (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   auto iter = begin;
   float width = stof(*iter++);
   float height = stof(*iter);
   return make_shared<isosceles> (GLfloat(width), GLfloat(height));
}

shape_ptr interpreter::make_equilateral (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   return make_shared<equilateral> (GLfloat(stof(*begin)));
}

