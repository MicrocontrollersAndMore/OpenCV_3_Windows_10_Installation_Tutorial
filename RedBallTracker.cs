// RedBallTracker.cs
//
// put this code in your main form, for example frmMain.cs
// add the following components to your form:
//
// ibOriginal (Emgu ImageBox)
// ibProcessed (Emgu ImageBox)
// btnPauseOrResume (Button)
// txtXYRadius (TextBox)
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

using Emgu.CV;                 //
using Emgu.CV.CvEnum;          // usual Emgu Cv imports
using Emgu.CV.Structure;       //
using Emgu.CV.UI;              //

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace RedBallTracker1
{
    public partial class frmMain : Form
    {
        // member variables ///////////////////////////////////////////////////////////////////////
        Capture capWebcam;                      // Capture object to use with webcam
        bool blnCapturingInProcess = false;     // use this to keep track of if we are capturing or not to facilitate pause/resume button feature

        Image<Bgr, Byte> imgOriginal;           // original image
        Image<Bgr, Byte> imgBlurredBGR;         // blurred color image
        Image<Gray, Byte> imgProcessed;         // processed image

        public frmMain()
        {
            InitializeComponent();
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        private void frmMain_Load(object sender, EventArgs e)
        {
            try
            {
                capWebcam = new Capture();          // associate the capture object to the default webcam
            }
            catch (Exception ex)                    // catch error if unsuccessful
            {                                       // show error via message box
                MessageBox.Show("unable to read from webcam, error: " + Environment.NewLine + Environment.NewLine +
                                ex.Message + Environment.NewLine + Environment.NewLine +
                                "exiting program");
                Environment.Exit(0);                // and exit program
            }
            Application.Idle += processFrameAndUpdateGUI;       // add process image function to the application's list of tasks
            blnCapturingInProcess = true;
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

            imgBlurredBGR = imgOriginal.SmoothGaussian(5);          // blur

            imgProcessed = imgBlurredBGR.InRange(new Bgr(0, 0, 175), new Bgr(100, 100, 256));       // filter on color

            imgProcessed = imgProcessed.SmoothGaussian(5);          // blur again

            StructuringElementEx structuringElementEx = new StructuringElementEx(5, 5, 1, 1, CV_ELEMENT_SHAPE.CV_SHAPE_RECT);       // declare structuring element to use in dilate and erode

            CvInvoke.cvDilate(imgProcessed, imgProcessed, structuringElementEx, 1);             // close image (dilate, then erode)
            CvInvoke.cvErode(imgProcessed, imgProcessed, structuringElementEx, 1);              // closing "closes" (i.e. fills in) foreground gaps

            CircleF[] circles = imgProcessed.HoughCircles(new Gray(100), new Gray(50), 2, imgProcessed.Height / 4, 10, 400)[0];     // fill variable circles with all circles in the processed image

            foreach (CircleF circle in circles)                     // for each circle
            {
                if (txtXYRadius.Text != "") txtXYRadius.AppendText(Environment.NewLine);        // if we are not on the first line in the text box then insert a new line char

                txtXYRadius.AppendText("ball position = x " + circle.Center.X.ToString().PadLeft(4) +           // print ball position and radius
                                       ", y = " + circle.Center.Y.ToString().PadLeft(4) +                       //
                                       ", radius = " + circle.Radius.ToString("###.000").PadLeft(7));           //

                txtXYRadius.ScrollToCaret();                // scroll down in text box so most recent line added (at the bottom) will be shown

                // draw a small green circle at the center of the detected object
                CvInvoke.cvCircle(imgOriginal, new Point((int)circle.Center.X, (int)circle.Center.Y), 3, new MCvScalar(0, 255, 0), -1, LINE_TYPE.CV_AA, 0);

                imgOriginal.Draw(circle, new Bgr(Color.Red), 3);        // draw a red circle around the detected object
            }
            ibOriginal.Image = imgOriginal;             // update image boxes on form
            ibProcessed.Image = imgProcessed;           //
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        private void btnPauseOrResume_Click(object sender, EventArgs e)
        {
            if (blnCapturingInProcess == true)                      // if we are currently processing an image, user just choose pause, so . . .
            {
                Application.Idle -= processFrameAndUpdateGUI;       // remove the process image function from the application's list of tasks
                blnCapturingInProcess = false;                      // update flag variable
                btnPauseOrResume.Text = " resume ";                 // update button text
            }
            else                                                    // else if we are not currently processing an image, user just choose resume, so . . .
            {
                Application.Idle += processFrameAndUpdateGUI;       // add the process image function to the application's list of tasks
                blnCapturingInProcess = true;                       // update flag variable
                btnPauseOrResume.Text = " pause ";                  // new button will offer pause option
            }
        }

    }
}
