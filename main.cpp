//*******************************************
//*C++ / OpenCV basic facial recognition    *
//*project. This program leverages OpenCV's *
//*native facial recognition features,      *
//*detects and displays a box around the    *
//*face                                     *
//*******************************************

//*******************************************
//*                Directives               *
//*******************************************
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <libavcodec/avcodec.h>
using namespace std;
using namespace cv;


//*******************************************
//*                CONSTANTS                *
//*******************************************
const string VIDEO_FILE = "/Users/michaelgamble/Movies/Sample_Videos/TOS_00-01-20_Min_Edit_WM.mov";
const string WRITE_PATH = "/Users/michaelgamble/Documents/XCode Projects/Learning OpenCV Exercises/Face_Detection/";
const string WINDOW_NAME = "Original";
const string FRONT_FACE = "/Users/michaelgamble/Documents/XCode Projects/Learning OpenCV Exercises/Face_Detection/lbpcascades/lbpcascade_frontalface_improved.xml";
const string PROFILE_FACE = "/Users/michaelgamble/Documents/XCode Projects/Learning OpenCV Exercises/Face_Detection/lbpcascades/lbpcascade_profileface.xml";

const double SCALE_FACTOR = 1.1;
const double MIN_NEIGHBOR = 3;
const Size   MIN_SIZE(30,
                      30);

//*******************************************
//*               STRUCTURES                *
//*******************************************
struct VidInfo                                      //Struct to hold VidInfo
{
    VideoCapture *cap = nullptr;                    //Pointer to the cap object
    double fourcc;                                  //Video codec
    double fps;                                     //Frame play speed
    Size frameSize;                                 //Frame Size w x h
    double runTime;                                 //Film run time
    CascadeClassifier frontFaceCascade;             //Used to detect front faces
    CascadeClassifier profileFaceCascade;           //Used to detect profile faces
};

//*******************************************
//*           Function Prototypes           *
//*******************************************
VideoCapture openCapture();                         //Opens and returns capture
struct VidInfo getVidInfo(VideoCapture cap);        //Returns struct of VidInfo
VideoWriter openWriter(VidInfo video);              //Opens and returns the writer
void faceDetection(Mat frames,                      //Detects faces and calls drawRect
                   vector<Rect>frontFaces,
                   VidInfo video);
void drawRect(Mat frames, vector<Rect> frontFaces); //Draws rectangles around detected faces


int main(int argc, char** argv)
{
    
    VideoCapture cap(0);                            //Create and open the catputre object by
                                                    //calling the openCapture() constructor
    
    VidInfo video = getVidInfo(cap);                //Get the VidInfo
    
    VideoWriter writer(openWriter(video));          //Open a writer, pass the struct
    
    Mat frames;                                     //Mat object to hold the frames
    
    vector<Rect> frontFaces;                        //Create a vector of rectangles to
                                                    //hold the coordinates of a face detected
                                                    //inside the image
    
    vector<Rect> profileFaces;
    
  
    
    if(!video.frontFaceCascade.load(FRONT_FACE) &&  //Load and test the front and
       !video.profileFaceCascade.load(PROFILE_FACE))//profile rules to detect faces
    {
        cerr  <<  "Failed to load cascade classifier. \n";
        cerr  <<  "please check that both lbpcascade_frontalface_improved.xml, ";
        cerr  <<  "and lbpcascade_profileface.xml are in the lbpcascades folder  ";
        cerr  <<  "located in the top leve of the project director, and try again ";
        cerr  <<  endl;
        
        return -1;
    }

    
    namedWindow(WINDOW_NAME,                        //Window to host output
                WINDOW_AUTOSIZE);                   //Auto size the window
    
    
    while(true)                                     //While still reading frames
    {
        cap  >> frames;                             //Read the frame to the array
        
        if(frames.empty())                          //Test for empty frame
        { break; }                                  //Break the loop
        
        faceDetection(frames, frontFaces, video);
        
        writer.write(frames);                       //Write the frame
        
        imshow(WINDOW_NAME, frames);                //Display the frame
        
        char c = waitKey((int)video.fps);           //Next frame > video.fps
        if(c == 27)                                 //or quit if [esc]
        { break; }                                  //and break the loop
        
    }
    
    //Clean up
    destroyWindow(WINDOW_NAME);                     //Close the window
    
    cout  <<  "End of video."  <<  endl  <<  endl;
    
    cap.release();                                  //Release the caputre memory
    
    return 0;
    
}
//*******************************************
//*faceDetection function: the arguments are*
//*Mat object frames and vector of Rect     *
//*coordinates, this function calls another *
//*function to draw the rectangles on the   *
//*vect                                     *
//*******************************************
void faceDetection(Mat frames, vector<Rect>frontFaces, VidInfo video)
{
    //The 'frontFaceCascade.detectMultiScale() function detects the faces in the image and
    //places rectangle corrdinates in the faces vector
    video.frontFaceCascade.detectMultiScale(
                                          frames,               //Input array / source image
                                          frontFaces,           //Faces vector, detection coordinates
                                          SCALE_FACTOR,         //double, scale factor, specifying how
                                          //much the image size is reduced at each image scale.
                                          MIN_NEIGHBOR,         //int minNeighbors = 3, specifying how many neighbors
                                          //each candidate rectangle should have to retain it.
                                          CASCADE_SCALE_IMAGE,  //CASCADE_SCALE_IMAGE flag
                                          MIN_SIZE              //Minimum possible object size < ignored
                                          //Optional maxSize > ignored
                                          );
    
    
    drawRect(frames, frontFaces);                               //Draw the rectangles
    
}

//*******************************************
//*drawRect function: the arguments are Mat *
//*object frame and a vector of rectangles  *
//*as coordinates of faces, the function    *
//*draws a rectangle around the detection   *
//*******************************************
void drawRect(Mat frames, vector<Rect> frontFaces)
{
    //This loop is used to draw a rectangle around the faces in the
    //faces vector
    for(size_t index = 0; index < frontFaces.size(); index++)
    {
        //Use the rectangle function to draw a red boarder around
        //the face corrdinates in faces vector, use a thickness of
        //2 for the boarder
        rectangle(                          //Function draw
                  frames,                   //Input array
                  frontFaces[index],            //Face coordiantes
                  Scalar(0, 0, 255),        //B,G,R
                  2);                       //Thickness
    }
}


//*******************************************
//*getVidInfo function: accepts a tested and*
//*open capture object and returns a struct *
//*containing the capture object video info *
//*******************************************
struct VidInfo getVidInfo(VideoCapture cap)
{
    VidInfo video;
    
    video.fps = cap.get(CAP_PROP_FPS);              //Get the FPS
    
    //Get the input frame size - width x height
    video.frameSize.width  = (int) cap.get(CAP_PROP_FRAME_WIDTH);
    video.frameSize.height = (int) cap.get(CAP_PROP_FRAME_HEIGHT);
    
    video.fourcc =  cap.get(CAP_PROP_FOURCC);       //Get the codec
    
    //Store the run time
    video.runTime = (cap.get(CAP_PROP_FRAME_COUNT) / video.fps);
    
    video.cap = &cap;                               //Reference to the capture
    
    //Print the video info
    cout  <<  "FPS: "  <<  video.fps  <<  endl;     //Print the video speed in fps
    cout  <<  "Frame Size: "                        //Print the frame size
    <<  "width = "
    << video.frameSize.width                        //Frame Width
    <<  " height = "
    << video.frameSize.height                       //Frame length
    <<  endl;
    cout  <<  "Run Time: "  <<  video.runTime  <<  "s "  <<  endl;
    
    return video;                                   //Return the struct
    
    }
    
    
    //*******************************************
    //*openCapture function: no parameters, test*
    //*and open a capture object initalized with*
    //*a file                                   *
    //*******************************************
    VideoCapture openCapture()
    {
        VideoCapture cap(VIDEO_FILE);               //Create a video capture object
        
        if(!cap.isOpened())                         //If capture device didn't open
        {
            cerr  <<  "Error: Video capture object failed to open."  <<  endl;
            cerr  <<  "Make sure the file name is correct, the camera is available \n";
            cerr  <<  "or that you have all necessary codecs installed and run this \n";
            cerr  <<  "program again."  <<  endl  <<  endl;
            
            cerr  <<  "Program terminated, good-bye!"  <<  endl;
            
            return -1;                              //Quit
        }
        
        return cap;
    }
    
    
    //*******************************************
    //*openWriter function: accepts a struct of *
    //*VidInfo, opens the writer, and tests the *
    //*to see if the writer opened              *
    //*******************************************
    VideoWriter openWriter(VidInfo video)
    {
       
        
        VideoWriter writer(                         //Create a VideoWriter object
                           WRITE_PATH +
                           "output.avi",            //Output file name
                           video.fourcc,            //Code for out file
                           video.fps,               //Playback speed
                           video.frameSize,         //Size of frames w x h
                           true  );                 //isColor? t or f
        
        if(!writer.isOpened())                      //If writer object didn't open
        {
            cerr  <<  "Error: Video writer object failed to open."  <<  endl;
            cerr  <<  "Make sure you have writer privleges to the output director \n";
            cerr  <<  "and all necessary codecs installed and run this program again."
            <<  endl  <<  endl;
            
            cerr  <<  "openWriter function terminated, no file written." <<  endl;
            
            EXIT_FAILURE;                           //Quit the function
                                                    //The program should still preview
                                                    //the file just not write
        }
        
        return writer;
    }
