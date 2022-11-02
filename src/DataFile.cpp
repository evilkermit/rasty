#include "DataFile.h"

#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <utility>

#include "gdal/gdal_priv.h"
#include "gdal/cpl_conv.h"
#include "ospray/ospray_cpp/ext/rkcommon.h"
// #include <CGAL/property_map.h>
// #include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
// #include <CGAL/Projection_traits_xy_3.h>
// #include <CGAL/Delaunay_triangulation_2.h>
// #include <CGAL/Triangulation_vertex_base_with_info_2.h>
// #include <CGAL/Triangulation_face_base_with_info_2.h>
// #include <CGAL/boost/graph/graph_traits_Delaunay_triangulation_2.h>
// #include <CGAL/boost/graph/copy_face_graph.h>
// #include <CGAL/Point_set_3.h>
// #include <CGAL/Point_set_3/IO.h>
// #include <CGAL/compute_average_spacing.h>
// #include <CGAL/Surface_mesh.h>
// #include <CGAL/Polygon_mesh_processing/locate.h>
// #include <CGAL/Polygon_mesh_processing/triangulate_hole.h>
// #include <CGAL/Polygon_mesh_processing/border.h>
// #include <CGAL/Polygon_mesh_processing/remesh.h>
// #include <boost/graph/adjacency_list.hpp>
// #include <CGAL/boost/graph/split_graph_into_polylines.h>
// #include <CGAL/IO/WKT.h>
// #include <CGAL/Constrained_Delaunay_triangulation_2.h>
// #include <CGAL/Constrained_triangulation_plus_2.h>
// #include <CGAL/Polyline_simplification_2/simplify.h>
// #include <CGAL/Polyline_simplification_2/Squared_distance_cost.h>
// #include <CGAL/Classification.h>
// #include <CGAL/Random.h>


namespace rasty
{

// typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
// typedef Kernel::FT FT;
// typedef Kernel::Point_3 Point;
// typedef Kernel::Vector_3 Vector;
// typedef CGAL::Point_set_3<Point> Point_set;

    // typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
    // typedef CGAL::Projection_traits_xy_3<Kernel> Projection_traits;
    // typedef Kernel::Point_2 Point_2;
    // typedef Kernel::Point_3 Point_3;
    // typedef Kernel::Segment_3 Segment_3;;

    // typedef Kernel::FT FT;

    // typedef Kernel::Vector_3 Vector;

    // // Triangulated Irregular Network
    // typedef CGAL::Delaunay_triangulation_2<Projection_traits> TIN;
    // typedef CGAL::Surface_mesh<Point_3> Mesh;
    // typedef CGAL::Point_set_3<Point_3> Point_set;
    // typedef CGAL::Triangulation_vertex_base_with_info_2 <Point_set::Index, Projection_traits> Vbi;
    // typedef CGAL::Triangulation_face_base_with_info_2 <int, Projection_traits> Fbi;
    // typedef CGAL::Triangulation_data_structure_2<Vbi, Fbi> TDS;
    // typedef CGAL::Delaunay_triangulation_2<Projection_traits, TDS> TIN_with_info;
    // typedef Mesh::Vertex_index vertex_descriptor;
    // typedef Mesh::Face_index face_descriptor;

DataFile::DataFile(): statsCalculated(false){
    std::cout << "[DataFile] Init" << std::endl;
    this->data = NULL;
}

DataFile::~DataFile(){
    std::cout << "[DataFile] Deleting DataFile" << std::endl;

    if (this->data != NULL){
        free(this->data);
    }

    this->data = NULL;
    std::cout << "[DataFile] Deleted DataFile" << std::endl;

}


void DataFile::loadFromFile(std::string filename)
{
    this->filename = filename;
    this->filetype = getFiletype();

    if(this->filetype == UNKNOWN) {
        std::cerr << "Unknown filetype!" << std::endl;
    }
    else if (this->filetype == TIFF) {
        readTIFF();
    }
    else if (this->filetype == NETCDF) {
        /* TODO */
    }
        
    // Mesh mesh;
    // std::ifstream input(this->filename,std::ios_base::binary);
    // if (!input)
    // {
    //     std::cerr << "[DataFile] Error: cannot read file " << this->filename << std::endl;
    //     return;
    // }

    // std::cout << "[DataFile] Reading file " << this->filename << std::endl;
    // CGAL::IO::read_PLY (input, mesh);
    // input.close();

    // std::cout<<"[DataFile] Number of points: "<< mesh.number_of_vertices() <<std::endl;
    // std::cout<<"[DataFile] Number of faces: "<< mesh.number_of_faces() <<std::endl;
    // std::cout<<"[DataFile] Number of edges: "<< mesh.number_of_edges() <<std::endl;
    // this->numValues = mesh.number_of_vertices();


    // std::cout << "[DataFile] Creating Vertices and Colors" << std::endl;
    // this->vertex.reserve(this->numValues);
    // this->color.reserve(this->numValues);
    // for (auto v : mesh.vertices())
    // {
    //     auto point = mesh.point(v);
    //     this->vertex.push_back(rkcommon::math::vec3f(point.x(), point.y(), point.z()));
    //     this->color.push_back(rkcommon::math::vec4f(229.f, 217.f, 194.f, 1.f));
    // }

    // std::cout << "[DataFile] Creating Indices" << std::endl;
    // this->index.reserve(mesh.number_of_faces());

    // //Get face indices ...
    // for (Mesh::Face_index face_index : mesh.faces()) {
    //     CGAL::Vertex_around_face_circulator<Mesh> vcirc(mesh.halfedge(face_index), mesh), done(vcirc);
    //     rkcommon::math::vec3ui face;
    //     face.x = *vcirc++;
    //     face.y = *vcirc++;
    //     face.z = *vcirc++;
    //     this->index.push_back(face);
    // }
    // std::cout << "[DataFile] Done" << std::endl;
}   

void DataFile::readTIFF()
{

    /* read in the raster data */
    GDALAllRegister();

    GDALDataset  *dataset;
    GDALRasterBand  *elevationBand;
    CPLErr err;
    int numIndices;

    /* open the file */
    dataset = (GDALDataset *) GDALOpen(filename.c_str(), GA_ReadOnly);
    if( dataset == NULL ) {
        std::cout << "Failed to open" << std::endl;
        exit(1);
    }

    /* get image metadata */
    this->width = dataset->GetRasterXSize();
    this->height = dataset->GetRasterYSize();
    this->numValues = this->width * this->height;
    if (dataset->GetGeoTransform(this->geoTransform) == CE_None ) {
        this->originX = this->geoTransform[0];
        this->originY = this->geoTransform[3];
        this->pixelSizeX = this->geoTransform[1];
        this->pixelSizeY = this->geoTransform[5];
    } else {
        std::cout << "Failed read geotransform" << std::endl;
        exit(1);
    }

    /* not memmaped */
    /* TODO - look into memmaping */

    /* get the reaster band (DEM has one raster band representing elevation) */
    elevationBand = dataset->GetRasterBand(1);


    /* read the raster band into an array */
    /* TODO - look into being able to scale this down need be */
    this->data = (float *)malloc(this->numValues * sizeof(float));

    err = elevationBand->RasterIO(GF_Read, 0, 0, this->width, this->height, &this->data[0], this->width, this->height, GDT_Float32, 0, 0);
    if (err != CE_None) {
        std::cout << "Failed to read raster band" << std::endl;
        exit(1);
    }

    /* close the dataset */
    GDALClose(dataset); 
    GDALDestroy();

    /* convert data set to vertex and color arrays */
    numIndices = (this->width - 1) * (this->height - 1) * 2;
    this->vertex.reserve(this->numValues);
    this->color.reserve(this->numValues);
    this->index.reserve(numIndices);
    for (int x = 0; x < this->height; x++) {
        for (int y = 0; y < this->width; y++) {

            /* get current coordinate */
            double currY = this->originY + (x * this->pixelSizeY);
            double currX = this->originX + (y * this->pixelSizeX);

            /* get the elevation value */
            double z = this->data[(x * this->width) +y];
            this->vertex.push_back(rkcommon::math::vec3d(currX, currY, z));
            // this->vertex.push_back(rkcommon::math::vec3d(x, y, z));
            this->color.push_back(rkcommon::math::vec4f(0.9f, 0.5f, 0.5f, 1.f));

            /* create indices for vertex array */
            if (x+1 < this->height && y+1 < this->width) {
                int br = ((x+1) * this->width) + (y+1);
                int tl = (x * this->width) + y;
                int tr = (x * this->width) + (y+1);
                int bl = ((x+1) * this->width) + y;
                this->index.push_back(rkcommon::math::vec3ui(tl,tr,br)); // top right triangle
                this->index.push_back(rkcommon::math::vec3ui(tl,bl,br)); // bottom left triangle
            }
        }
    }
}

FILETYPE DataFile::getFiletype()
{
    std::stringstream ss;
    ss.str(this->filename);
    std::string token;
    char delim = '.';
    // keep the last token after splitting on dots
    while(std::getline(ss, token, delim)) {
    }

    if (token.compare("tiff") == 0 || 
        token.compare("tif") == 0) {
        return TIFF;
    }
    // if(token.compare("bin") == 0 ||
    //    token.compare("dat") == 0 ||
    //    token.compare("raw") == 0) {
    //     return BINARY;
    // }
    else if(token.compare("nc") == 0) {
        return NETCDF;
    }
    else {
        return UNKNOWN;
    }
}

void DataFile::calculateStatistics()
{
    // calculate min, max, avg, stddev
    // stddev and avg may be useful for automatic diverging color maps
    this->minVal = data[0];
    this->maxVal = data[0];
    double total = 0, totalSquares = 0;
    for(int i = 1; i < this->numValues; i++) {
        if(data[i] < this->minVal)
            this->minVal = data[i];
        if(data[i] > this->maxVal)
            this->maxVal = data[i];
        total += data[i];
        totalSquares += data[i]*data[i];
    }
    this->avgVal = total / this->numValues;
    this->stdDev = std::sqrt(totalSquares/this->numValues -
                             this->avgVal*this->avgVal);
    this->statsCalculated = true;
}


void DataFile::printStatistics()
{
    // debugging purposes
    if(!this->statsCalculated) {
        std::cerr << "Statistics not calculated for this data!" << std::endl;
        return;
    }

    std::cout << this->filename << std::endl;
    // std::cout << "dimensions: " << this->xDim << ", "
    //                             << this->yDim << ", "
    //                             << this->zDim << std::endl;
    std::cout << "minimum:    " << this->minVal << std::endl;
    std::cout << "maximum:    " << this->maxVal << std::endl;
    std::cout << "mean:       " << this->avgVal << std::endl;
    std::cout << "std. dev.:  " << this->stdDev << std::endl;
}

}
