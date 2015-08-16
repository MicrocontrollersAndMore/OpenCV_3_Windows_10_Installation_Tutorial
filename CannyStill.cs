// CannyStill.cs
//
// put this code in your main form, for example frmMain.cs
// add the following components to your form:
//
// btnOpenFile (Button)
// lblChosenFile (Label)
// ibOriginal (Emgu ImageBox)
// ibCanny (Emgu ImageBox)
// ofdOpenFile (OpenFileDialog)
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
using Emgu.CV.CvEnum;           // usual Emgu Cv imports
using Emgu.CV.Structure;        //
using Emgu.CV.UI;               //

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace CannyStill1
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    public partial class frmMain : Form
    {
        // member variables ///////////////////////////////////////////////////////////////////////
        Image<Bgr, Byte> imgOriginal;           // input image
        Image<Gray, Byte> imgGrayscale;         // grayscale of input image
        Image<Gray, Byte> imgBlurred;           // intermediate blured image
        Image<Gray, Byte> imgCanny;             // Canny edge image

        int intButtonAndLabelHorizPadding;      //
        int intImageBoxesHorizPadding;          // original component padding for component resizing
        int intImageBoxesVertPadding;           //

        // constructor ////////////////////////////////////////////////////////////////////////////
        public frmMain()
        {
            InitializeComponent();

            intButtonAndLabelHorizPadding = this.Width - btnOpenFile.Width - lblChosenFile.Width;       //
            intImageBoxesHorizPadding = this.Width - ibOriginal.Width - ibCanny.Width;                  // get original padding for component resizing later
            intImageBoxesVertPadding = this.Height - btnOpenFile.Height - ibOriginal.Height;            //
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        private void frmMain_Resize(object sender, EventArgs e)
        {
            lblChosenFile.Width = this.Width - btnOpenFile.Width - intButtonAndLabelHorizPadding;       // resize label

            ibOriginal.Width = Convert.ToInt32((this.Width - intImageBoxesHorizPadding) / 2);           // resize image box widths
            ibCanny.Width = ibOriginal.Width;                                                           //

            ibCanny.Left = ibOriginal.Width + Convert.ToInt32(intImageBoxesHorizPadding * (1.0 / 2.5));     // update x position for Canny image box

            ibOriginal.Height = this.Height - btnOpenFile.Height - intImageBoxesVertPadding;            // resize image box heights
            ibCanny.Height = ibOriginal.Height;                                                         //
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        private void btnOpenFile_Click(object sender, EventArgs e)
        {
            DialogResult drChosenFile;

            drChosenFile = ofdOpenFile.ShowDialog();            // open file dialog

            if (drChosenFile != System.Windows.Forms.DialogResult.OK || ofdOpenFile.FileName == "")     // if user chose Cancel or filename is blank . . .
            {
                lblChosenFile.Text = "file not chosen";             // show error message on label
                return;                                             // and exit function
            }

            try
            {
                imgOriginal = new Image<Bgr, byte>(ofdOpenFile.FileName);       // open image
            }
            catch (Exception exception)                                         // if error occurred
            {
                lblChosenFile.Text = "unable to open image, error: " + exception.Message;       // show error message on label
                return;                                                                         // and exit function
            }

            if (imgOriginal == null)                                    // if image could not be opened
            {
                lblChosenFile.Text = "unable to open image";            // show error message on label
                return;                                                 // and exit function
            }

            imgGrayscale = imgOriginal.Convert<Gray, Byte>();           // convert to grayscale
            imgBlurred = imgGrayscale.SmoothGaussian(5);                // blur

            double dblCannyThresh = 180.0;                              // declare params for call to Canny
            double dblCannyThreshLinking = 120.0;                       //

            imgCanny = imgBlurred.Canny(dblCannyThresh, dblCannyThreshLinking);         // get Canny edges

            ibOriginal.Image = imgOriginal;                 // update image boxes
            ibCanny.Image = imgCanny;                       //

        }

    }
}
