// CannyStill.cs
//
// Emgu CV 3.0.0
//
// put this code in your main form, for example frmMain.cs
// add the following components to your form:
//
// tlpOuter (TableLayoutPanel)
// tlpInner (TableLayoutPanel)
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
namespace CannyStill1 {

    ///////////////////////////////////////////////////////////////////////////////////////////////
    public partial class frmMain : Form {

        // constructor ////////////////////////////////////////////////////////////////////////////
        public frmMain() {
            InitializeComponent();
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        private void btnOpenFile_Click(object sender, EventArgs e) {
            DialogResult drChosenFile;

            drChosenFile = ofdOpenFile.ShowDialog();

            if (drChosenFile != DialogResult.OK || ofdOpenFile.FileName == "") {
                lblChosenFile.Text = "file not chosen";
                return;
            }

            Mat imgOriginal;

            try {
                imgOriginal = new Mat(ofdOpenFile.FileName, LoadImageType.Color);
            } catch (Exception ex) {
                lblChosenFile.Text = "unable to open image, error: " + ex.Message;
                return;
            }

            if (imgOriginal == null) {
                lblChosenFile.Text = "unable to open image";
                return;
            }

            Mat imgGrayscale = new Mat(imgOriginal.Size, DepthType.Cv8U, 1);
            Mat imgBlurred = new Mat(imgOriginal.Size, DepthType.Cv8U, 1);
            Mat imgCanny = new Mat(imgOriginal.Size, DepthType.Cv8U, 1);

            CvInvoke.CvtColor(imgOriginal, imgGrayscale, ColorConversion.Bgr2Gray);

            CvInvoke.GaussianBlur(imgGrayscale, imgBlurred, new Size(5, 5), 1.5);

            CvInvoke.Canny(imgBlurred, imgCanny, 100, 200);

            ibOriginal.Image = imgOriginal;
            ibCanny.Image = imgCanny;
        }

    }   // end class

}   // end namespace
