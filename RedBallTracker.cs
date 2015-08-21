// RedBallTracker.vb
//
// Emgu CV 3.0.0
//
// put this code in your main form, for example frmMain.vb
//
// add the following components to your form:
// tlpOuter (TableLayoutPanel)
// tlpInner (TableLayoutPanel)
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

using Emgu.CV;                  //
using Emgu.CV.CvEnum;           // usual Emgu CV imports
using Emgu.CV.Structure;        //
using Emgu.CV.UI;               //


///////////////////////////////////////////////////////////////////////////////////////////////////
namespace RedBallTracker1 {

    ///////////////////////////////////////////////////////////////////////////////////////////////
    public partial class frmMain : Form {

        // member variables ///////////////////////////////////////////////////////////////////////
        Capture capWebcam;
        bool blnCapturingInProcess = false;

        // constructor ////////////////////////////////////////////////////////////////////////////
        public frmMain() {
            InitializeComponent();
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        private void frmMain_Load(object sender, EventArgs e) {
            try {
                capWebcam = new Capture();
            } catch(Exception ex) {
                MessageBox.Show("unable to read from webcam, error: " + Environment.NewLine + Environment.NewLine +
                                ex.Message + Environment.NewLine + Environment.NewLine +
                                "exiting program");
                Environment.Exit(0);
                return;
            }
            Application.Idle += processFrameAndUpdateGUI;       // add process image function to the application's list of tasks
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        void processFrameAndUpdateGUI(object sender, EventArgs arg) {
            Mat imgOriginal;

            imgOriginal = capWebcam.QueryFrame();

            if(imgOriginal == null) {
                MessageBox.Show("unable to read from webcam" + Environment.NewLine + Environment.NewLine +
                                "exiting program");
                Environment.Exit(0);
                return;
            }

            Mat imgHSV = new Mat(imgOriginal.Size, DepthType.Cv8U, 3);

            Mat imgThreshLow = new Mat(imgOriginal.Size, DepthType.Cv8U, 1);
            Mat imgThreshHigh = new Mat(imgOriginal.Size, DepthType.Cv8U, 1);

            Mat imgThresh = new Mat(imgOriginal.Size, DepthType.Cv8U, 1);

            CvInvoke.CvtColor(imgOriginal, imgHSV, ColorConversion.Bgr2Hsv);

            CvInvoke.InRange(imgHSV, new ScalarArray(new MCvScalar(0, 155, 155)), new ScalarArray(new MCvScalar(15, 255, 255)), imgThreshLow);
            CvInvoke.InRange(imgHSV, new ScalarArray(new MCvScalar(160, 155, 155)), new ScalarArray(new MCvScalar(179, 255, 255)), imgThreshHigh);

            CvInvoke.Add(imgThreshLow, imgThreshHigh, imgThresh);

            CvInvoke.GaussianBlur(imgThresh, imgThresh, new Size(3, 3), 0);

            Mat structuringElement = CvInvoke.GetStructuringElement(ElementShape.Rectangle, new Size(3, 3), new Point(-1, -1));

            CvInvoke.Dilate(imgThresh, imgThresh, structuringElement, new Point(-1, -1), 1, BorderType.Default, new MCvScalar(0, 0, 0));
            CvInvoke.Erode(imgThresh, imgThresh, structuringElement, new Point(-1, -1), 1, BorderType.Default, new MCvScalar(0, 0, 0));
            
            CircleF[] circles = CvInvoke.HoughCircles(imgThresh, HoughType.Gradient, 2.0, imgThresh.Rows / 4, 100, 50, 10, 400);

            foreach(CircleF circle in circles) {
                if (txtXYRadius.Text != "") {                         // if we are not on the first line in the text box
                    txtXYRadius.AppendText(Environment.NewLine);         // then insert a new line char
                }

                txtXYRadius.AppendText("ball position x = " + circle.Center.X.ToString().PadLeft(4) + ", y = " + circle.Center.Y.ToString().PadLeft(4) + ", radius = " + circle.Radius.ToString("###.000").PadLeft(7));
                txtXYRadius.ScrollToCaret();             // scroll down in text box so most recent line added (at the bottom) will be shown

                CvInvoke.Circle(imgOriginal, new Point((int)circle.Center.X, (int)circle.Center.Y), (int)circle.Radius, new MCvScalar(0, 0, 255), 2);
                CvInvoke.Circle(imgOriginal, new Point((int)circle.Center.X, (int)circle.Center.Y), 3, new MCvScalar(0, 255, 0), -1);
            }
            ibOriginal.Image = imgOriginal;
            ibProcessed.Image = imgThresh;
        }
        
        ///////////////////////////////////////////////////////////////////////////////////////////
        private void btnPauseOrResume_Click(object sender, EventArgs e) {
            if (blnCapturingInProcess == true) {                    // if we are currently processing an image, user just choose pause, so . . .
                Application.Idle -= processFrameAndUpdateGUI;       // remove the process image function from the application's list of tasks
                blnCapturingInProcess = false;                      // update flag variable
                btnPauseOrResume.Text = " resume ";                 // update button text
            } else {                                                // else if we are not currently processing an image, user just choose resume, so . . .
                Application.Idle += processFrameAndUpdateGUI;       // add the process image function to the application's list of tasks
                blnCapturingInProcess = true;                       // update flag variable
                btnPauseOrResume.Text = " pause ";                  // new button will offer pause option
            }
        }
        
    }   // end class

}   // end namespace
