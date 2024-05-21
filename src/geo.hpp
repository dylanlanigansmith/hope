#pragma once
#include <common.hpp>
#include <SDL3/SDL_video.h>

#include "geojson.hh"


namespace geo
{


static int loadgeo(geojson_t& gj, const char* filename){
   
 

    if(gj.convert(filename) != 0){
        ERRORF("failed loading GeoJSON file %s", filename);
        return 1;
    }
    //this worked first try
    //so damn
    LOGF("loaded GeoJSON %s size = %li", filename, gj.m_feature.size());
    
    

    //stolen, gets bounds 
    double x_low, y_low, x_high, y_high; //data
    x_high = x_low = y_high = y_low = 0.00; 

     size_t size_features = gj.m_feature.size();
  for (size_t idx_fet = 0; idx_fet < size_features; idx_fet++)
  {
    feature_t feature = gj.m_feature.at(idx_fet);
    size_t size_geometry = feature.m_geometry.size();
    for (size_t idx_geo = 0; idx_geo < size_geometry; idx_geo++)
    {
      geometry_t geometry = feature.m_geometry.at(idx_geo);
      size_t size_pol = geometry.m_polygons.size();
      for (size_t idx_pol = 0; idx_pol < size_pol; idx_pol++)
      {
        polygon_t polygon = geometry.m_polygons[idx_pol];
        size_t size_crd = polygon.m_coord.size();
        if (size_crd == 0)
        {
          continue;
        }
        std::vector<double> lat;
        std::vector<double> lon;
        for (size_t idx = 0; idx < size_crd; idx++)
        {
          lat.push_back(polygon.m_coord[idx].y);
          lon.push_back(polygon.m_coord[idx].x);
        }
        for (size_t idx = 0; idx < size_crd; idx++)
        {
          double lat_ = lat.at(idx);
          double lon_ = lon.at(idx);
          if (lat_ > y_high)
          {
            y_high = lat.at(idx);
          }
          if (lon_ > x_high)
          {
            x_high = lon.at(idx);
          }
          if (lat_ < y_low)
          {
            y_low = lat.at(idx);
          }
          if (lon_ < x_low)
          {
            x_low = lon.at(idx);
          }
        }
      }  //idx_pol
    } //idx_geo
  } //idx_fet

  LOGF("x_low=%f x_high=%f y_low=%f y_high=%f", x_low, x_high, y_low, y_high);
    return 0;
}


static int load(geojson_t& gj, const char* filename){
   
 

    if(gj.convert(filename) != 0){
        ERRORF("failed loading GeoJSON file %s", filename);
        return 1;
    }
    //this worked first try
    //so damn
    LOGF("loaded GeoJSON %s size = %li", filename, gj.m_feature.size());
    


    //stolen, gets bounds 
    double x_low, y_low, x_high, y_high; //data
    x_high = x_low = y_high = y_low = 0.00; 

     size_t size_features = gj.m_feature.size();
  for (size_t idx_fet = 0; idx_fet < size_features; idx_fet++)
  {
    feature_t feature = gj.m_feature.at(idx_fet);
    size_t size_geometry = feature.m_geometry.size();
    for (size_t idx_geo = 0; idx_geo < size_geometry; idx_geo++)
    {
      geometry_t geometry = feature.m_geometry.at(idx_geo);
      size_t size_pol = geometry.m_polygons.size();
      for (size_t idx_pol = 0; idx_pol < size_pol; idx_pol++)
      {
        polygon_t polygon = geometry.m_polygons[idx_pol];
        size_t size_crd = polygon.m_coord.size();
        if (size_crd == 0)
        {
          continue;
        }
        std::vector<double> lat;
        std::vector<double> lon;
        for (size_t idx = 0; idx < size_crd; idx++)
        {
          lat.push_back(polygon.m_coord[idx].y);
          lon.push_back(polygon.m_coord[idx].x);
        }
        for (size_t idx = 0; idx < size_crd; idx++)
        {
          double lat_ = lat.at(idx);
          double lon_ = lon.at(idx);
          if (lat_ > y_high)
          {
            y_high = lat.at(idx);
          }
          if (lon_ > x_high)
          {
            x_high = lon.at(idx);
          }
          if (lat_ < y_low)
          {
            y_low = lat.at(idx);
          }
          if (lon_ < x_low)
          {
            x_low = lon.at(idx);
          }
        }
      }  //idx_pol
    } //idx_geo
  } //idx_fet

  LOGF("x_low=%f x_high=%f y_low=%f y_high=%f", x_low, x_high, y_low, y_high);
    return 0;
}

struct geo_poly_t
{
    std::vector<SDL_FPoint> p;
    SDL_Color clr;
    std::string name;
};

static void render(geojson_t& gj, SDL_Renderer* render, double scale = 1.0){
    

    if(gj.m_feature.empty()) return;

    size_t size_features = gj.m_feature.size();
    size_t range = 1;
    size_t idx_pal = 0;


    static std::unordered_map<std::string, geo_poly_t> geo;

    static double last_scale = scale;

    if(last_scale != scale){
        geo.clear();
    }   
    last_scale = scale;
    ImGuiIO &io = ImGui::GetIO();  (void)io;
    ImGui::Begin(gj.m_feature.front().m_name.c_str());

    static float x_off =  0.f, y_off = 0.f; // -2750.f, y_off = -677.f;
    ImGui::SliderFloat("XOff", &x_off, -5000, 5000);
    ImGui::SliderFloat("YOff", &y_off, -5000, 5000);
    bool dragging = ImGui::IsMouseDragging(ImGuiMouseButton_Left);
    static bool last_drag = dragging;

    static float dragfactor = 0.01f;
    ImGui::SliderFloat("Drag", &dragfactor, 0.001f, 1.f);
     if(dragging){
        auto delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);

        
        x_off += delta.x * dragfactor;
        y_off += delta.y * dragfactor;
       // EM_ASM("alert(\"MOUSE ON THE MOVE\")");
    }
    bool drag_end = ((last_drag != dragging) && last_drag) && !io.WantCaptureMouse;
    last_drag = dragging;
 
    if(geo.empty() || ImGui::Button("Regenerate") || drag_end ){
        geo.clear();
        SDL_SetRenderDrawColor(render, 0,255,255,255);
        LOGF("%s","making geo render data");
        for (size_t idx_fet = 0; idx_fet < size_features; idx_fet++)
        {
            feature_t feature = gj.m_feature.at(idx_fet);
            
            auto& gp = geo[feature.m_name];
             gp.name = feature.m_name;
                         gp.clr = { (uint8_t)(int)(emscripten_random() * 255), (uint8_t)(int)(emscripten_random() * 255), (uint8_t)(int)(emscripten_random() * 255), 255 };
            size_t size_geometry = feature.m_geometry.size();

           
           
        //    LOGF("doin %s", feature.m_name.c_str());
           
            for (size_t idx_geo = 0; idx_geo < size_geometry; idx_geo++)
            {
                geometry_t geometry = feature.m_geometry.at(idx_geo);
             
                size_t size_pol = geometry.m_polygons.size();
                for (size_t idx_pol = 0; idx_pol < size_pol; idx_pol++)
                {
                    polygon_t polygon = geometry.m_polygons[idx_pol];
                    size_t size_crd = polygon.m_coord.size();
                    if (size_crd == 0)
                    {
                        continue;
                    }
                    std::vector<double> lat;
                    std::vector<double> lon;
                    for (size_t idx_crd = 0; idx_crd < size_crd; idx_crd++)
                    {
                        lat.push_back(polygon.m_coord[idx_crd].y);
                        lon.push_back(polygon.m_coord[idx_crd].x);
                    }

                    if (geometry.m_type.compare("Point") == 0)
                    {
                    }
                    else if (geometry.m_type.compare("Polygon") == 0 ||
                             geometry.m_type.compare("MultiPolygon") == 0)
                    {
                        
                        const double hz = 180.0;
                        const double vz = 87.0;
                       

                        for (size_t idx_crd = 0; idx_crd < size_crd; idx_crd++)
                        {
                            SDL_FPoint point = { 
                                (float)( (lon.at(idx_crd) + hz) * scale + x_off), 
                                (float)( (-1.0 * lat.at(idx_crd) + vz) * scale + y_off)
                            };
                            gp.p.push_back(point);
                           
                        }
        
                    }
                } // idx_pol
            } // idx_geo
        } // idx_fet

        for(auto& entry : geo){
            auto& gp = entry.second;
            LOGF("%s with %li", gp.name.c_str(), gp.p.size());

            if(gp.p.size() % 2){
                gp.p.push_back(gp.p.front());
            }
        }
    }
    
    for(auto& entry : geo){
      //  if(entry.first != "Canada" ) continue;
        auto& gp = entry.second;
         SDL_SetRenderDrawColor(render, gp.clr.r, gp.clr.g,gp.clr.b, 255);
        // SDL_RenderLines(render, gp.p.data(), gp.p.size());
         // SDL_SetRenderDrawColor(render, 255,255,255,255);
          SDL_RenderPoints(render, gp.p.data(), gp.p.size());

         
          
    }
    /*
      for(auto& entry : geo){
          std::vector<SDL_Vertex> vert;
           auto& gp = entry.second;
          for(auto& p : entry.second.p){
            SDL_Vertex v;
            v.position = {p.x, p.y};
            v.color = {gp.clr.r / 255.f, gp.clr.g / 255.f,gp.clr.b / 255.f, 1.f};
            vert.push_back(v);
          }
          if(vert.size() % 3){
            //ahh
            continue;
          }
          int indices[] = {0,1,2};
          SDL_RenderGeometry(render,0,vert.data(), vert.size(),0,vert.size());

      }*/


    ImGui::End();
   //  SDL_RenderPoints(render, points.data(), points.size());

}


   

}



