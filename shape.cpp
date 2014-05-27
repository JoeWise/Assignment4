// $Id: shape.cpp,v 1.7 2014-05-08 18:32:56-07 - - $

#include <typeinfo>
#include <unordered_map>
#include <math.h>
using namespace std;

#include "shape.h"
#include "util.h"

const double PI = 3.141592653589793;

static unordered_map<void*,string> fontname {
   {GLUT_BITMAP_8_BY_13       , "Fixed-8x13"    },
   {GLUT_BITMAP_9_BY_15       , "Fixed-9x15"    },
   {GLUT_BITMAP_HELVETICA_10  , "Helvetica-10"  },
   {GLUT_BITMAP_HELVETICA_12  , "Helvetica-12"  },
   {GLUT_BITMAP_HELVETICA_18  , "Helvetica-18"  },
   {GLUT_BITMAP_TIMES_ROMAN_10, "Times-Roman-10"},
   {GLUT_BITMAP_TIMES_ROMAN_24, "Times-Roman-24"},
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

ostream& operator<< (ostream& out, const vertex& where) {
   out << "(" << where.xpos << "," << where.ypos << ")";
   return out;
}

shape::shape() {
   DEBUGF ('c', this);
}

text::text (void* glut_bitmap_font, const string& textdata):
      glut_bitmap_font(glut_bitmap_font), textdata(textdata) {
   DEBUGF ('c', this);
}

ellipse::ellipse (GLfloat width, GLfloat height):
dimension ({width, height}) {
   DEBUGF ('c', this);
}

circle::circle (GLfloat diameter): ellipse (diameter, diameter) {
   DEBUGF ('c', this);
}


rectangle::rectangle (GLfloat width, GLfloat height):
            polygon({}) {
   DEBUGF ('c', this << "(" << width << "," << height << ")");
   vertex v1;
   v1.xpos=0;
   v1.ypos=0;
   vertices.push_back(v1);

   vertex v2;
   v2.xpos=width;
   v2.ypos=0;
   vertices.push_back(v2);

   vertex v3;
   v3.xpos=width;
   v3.ypos=height;
   vertices.push_back(v3);

   vertex v4;
   v4.xpos=0;
   v4.ypos=height;
   vertices.push_back(v4);
}

square::square (GLfloat width): rectangle (width, width) {
   DEBUGF ('c', this);
}

diamond::diamond (GLfloat width, GLfloat height):
            polygon({}) {
   DEBUGF ('c', this << "(" << width << "," << height << ")");
   vertex v1;
   v1.xpos=0;
   v1.ypos=height / 2;
   vertices.push_back(v1);

   vertex v2;
   v2.xpos=width/2;
   v2.ypos=0;
   vertices.push_back(v2);

   vertex v3;
   v3.xpos=width;
   v3.ypos=height/2;
   vertices.push_back(v3);

   vertex v4;
   v4.xpos=width/2;
   v4.ypos=height;
   vertices.push_back(v4);
}

polygon::polygon (const vertex_list& vertices){
   DEBUGF ('c', this);
   vertex_list adjusted_verticies;
   int v_count = 0;
   int x_total = 0;
   int y_total = 0;
   for (auto iter = vertices.cbegin(); iter != vertices.cend(); ++iter){
      v_count++;
      x_total += iter->xpos;
      y_total += iter->ypos;
   }
   int center_x = x_total/v_count;
   int center_y = y_total/v_count;

   for (auto iter = vertices.cbegin(); iter != vertices.cend(); ++iter){
      vertex v;
      v.xpos = iter->xpos - center_x;
      v.ypos = iter->ypos - center_y;
      this->vertices.push_back(v);
   }
}

triangle::triangle (const vertex_list& verts):
            polygon({}) {
   DEBUGF ('c', this);
   vertices = verts;
}

right_triangle::right_triangle (GLfloat width, GLfloat height):
            triangle({}) {
   DEBUGF ('c', this << "(" << width << "," << height << ")");
   vertex v1;
   v1.xpos=0;
   v1.ypos=0;
   vertices.push_back(v1);

   vertex v2;
   v2.xpos=width;
   v2.ypos=0;
   vertices.push_back(v2);

   vertex v3;
   v3.xpos=0;
   v3.ypos=height;
   vertices.push_back(v3);
}

isosceles::isosceles (GLfloat width, GLfloat height):
            triangle({}) {
   DEBUGF ('c', this << "(" << width << "," << height << ")");
   vertex v1;
   v1.xpos=0;
   v1.ypos=0;
   vertices.push_back(v1);

   vertex v2;
   v2.xpos=width/2;
   v2.ypos=height;
   vertices.push_back(v2);

   vertex v3;
   v3.xpos=width;
   v3.ypos=0;
   vertices.push_back(v3);
}

equilateral::equilateral (GLfloat width):
            triangle({}) {
   DEBUGF ('c', this);
   vertex v1;
   v1.xpos=0;
   v1.ypos=0;
   vertices.push_back(v1);

   vertex v2;
   v2.xpos=width/2;
   v2.ypos=sqrt(pow(width,2)+pow(width/2,2));
   vertices.push_back(v2);

   vertex v3;
   v3.xpos=width;
   v3.ypos=0;
   vertices.push_back(v3);
}

void text::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
}

void ellipse::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glBegin (GL_POLYGON);
   glEnable (GL_LINE_SMOOTH);
   glColor3d(color.ubvec[0], color.ubvec[1], color.ubvec[2]);
   const float delta = 2 * PI / 32;
   for (float theta = 0; theta < 2 * PI; theta += delta) {
      float xdraw = dimension.xpos * cos (theta) + center.xpos;
      float ydraw = dimension.ypos * sin (theta) + center.xpos;
      glVertex2f (xdraw, ydraw);
   }
   glEnd();

}

void polygon::draw (const vertex& center, const rgbcolor& color) const{
   DEBUGF ('d', this << "(" << center << "," << color << ")");

   //tell GL we're gonna start drawing a polygon
   glBegin(GL_POLYGON);

   for(size_t i=0; i<vertices.size(); ++i)
   {
      glColor3d(color.ubvec[0], color.ubvec[1], color.ubvec[2]);
      glVertex2f(vertices[i].xpos+center.xpos,
               vertices[i].ypos+center.ypos);
   }


   glEnd();

}

void shape::show (ostream& out) const {
   out << this << "->" << demangle (*this) << ": ";
}

void text::show (ostream& out) const {
   shape::show (out);
   out << glut_bitmap_font << "(" << fontname[glut_bitmap_font]
       << ") \"" << textdata << "\"";
}

void ellipse::show (ostream& out) const {
   shape::show (out);
   out << "{" << dimension << "}";
}

void polygon::show (ostream& out) const {
   shape::show (out);
   out << "{" << vertices << "}";
}

ostream& operator<< (ostream& out, const shape& obj) {
   obj.show (out);
   return out;
}

