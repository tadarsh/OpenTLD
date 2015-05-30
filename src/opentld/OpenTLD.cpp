/*  Copyright 2011 AIT Austrian Institute of Technology
*
*   This file is part of OpenTLD.
*
*   OpenTLD is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   OpenTLD is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with OpenTLD.  If not, see <http://www.gnu.org/licenses/>.
*
*/

/**
  * @author Georg Nebehay
  */

/**
 * @modified by Adarsh A Tadimari (www.adarshtadimari.com)
 * Usage:
 * ./opentld [video-file]
 */

#include "Main.h"
#include "Config.h"
#include "ImAcq.h"
#include "Gui.h"
#include "FaceDetection.h"

using tld::Config;
using tld::Gui;
using tld::Settings;

int main(int argc, char **argv)
{

    Main *main = new Main();
    Config config;
    ImAcq *imAcq = imAcqAlloc();
    Gui *gui = new Gui();

    main->gui = gui;
    main->imAcq = imAcq;
    
    // Setting the input video path
    config.init_video(std::string(argv[1]));

    //if(config.init(argc, argv) == PROGRAM_EXIT)
    //{
    //    return EXIT_FAILURE;
    //}

    // Enforcing the configurations
    config.configure(main);

    srand(main->seed);

    // Initializing image acquisition
    imAcqInit(imAcq);

    if(main->showOutput)
    {
        gui->init();
    }

    // Tracking
    main->doWork();

    delete main;
    main = NULL;
    delete gui;
    gui = NULL;

    return EXIT_SUCCESS;
}
