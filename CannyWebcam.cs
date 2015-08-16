// CannyWebcam.cs
//
// put this code in your main form, for example frmMain.cs
// add the following components to your form:
//
// TableLayoutPanel1 (TableLayoutPanel) (name does not really matter for this, we will not need to refer to it in code
// ibOriginal (Emgu ImageBox)
// ibCanny (Emgu ImageBox)
//
// NOTE: Do NOT copy/paste the entire text of this file into Visual Studio !! It will not work if you do !!
// Follow the video on my YouTube channel to create the project and have Visual Studio write part of the code for you,
// then copy/pase the remaining text as needed

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

using Emgu.CV;                  //
using Emgu.CV.CvEnum;           // usual Emgu CV imports
using Emgu.CV.Structure;        //
using Emgu.CV.UI;               //

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace CannyWebcam
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    public partial class frmMain : Form
    {
        // member variables ///////////////////////////////////////////////////////////////////////
        Capture capWebcam;                      // Capture object to use with webcam

        Image<Bgr, Byte> imgOriginal;           // input image
        Image<Gray, Byte> imgGrayscale;         // grayscale of input image
        Image<Gray, Byte> imgBlurred;           // intermediate blured image
        Image<Gray, Byte> imgCanny;             // Canny edge image

        // constructor ////////////////////////////////////////////////////////////////////////////
        public frmMain()
        {
            InitializeComponent();
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        private void frmMain_Load(object sender, EventArgs e) 
        {
            try
            {
                capWebcam = new Capture();              // associate the capture object to the default webcam
            }
            catch (Exception ex)                        // catch error if unsuccessful
            {                                           // show error via message box
                MessageBox.Show("unable to read from webcam, error: " + Environment.NewLine + Environment.NewLine +
                                ex.Message + Environment.NewLine + Environment.NewLine +
                                "exiting program");
                Environment.Exit(0);                    // and exit program
            }
            Application.Idle += processFrameAndUpdateGUI;       // add process image function to the application's list of tasks
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        void processFrameAndUpdateGUI(object sender, EventArgs arg)
        {
            imgOriginal = capWebcam.QueryFrame();               // get next frame from the webcam

            if (imgOriginal == null)                            // if we did not get a frame
            {                                                   // show error via message box
                MessageBox.Show("unable to read from webcam" + Environment.NewLine + Environment.NewLine +
                                "exiting program");
                Environment.Exit(0);                            // and exit program
            }
            imgGrayscale = imgOriginal.Convert<Gray, Byte>();       // convert to grayscale
            imgBlurred = imgGrayscale.SmoothGaussian(5);            // blur

            double dblCannyThresh = 150.0;                          // declare params for call to Canny
            double dblCannyThreshLinking = 75.0;                    //

            imgCanny = imgBlurred.Canny(dblCannyThresh, dblCannyThreshLinking);     // get Canny edges

            ibOriginal.Image = imgOriginal;             // update image boxes
            ibCanny.Image = imgCanny;                   //
        }
    }
}
