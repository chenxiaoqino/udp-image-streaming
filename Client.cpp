/*
 *   C++ sockets on Unix and Windows
 *   Copyright (C) 2002
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "PracticalSocket.h"      // For UDPSocket and SocketException
#include <iostream>               // For cout and cerr
#include <cstdlib>                // For atoi()

using namespace std;

#include "opencv2/opencv.hpp"
using namespace cv;
#include "config.h"


int main(int argc, char *argv[]) {
  if ((argc < 3) || (argc > 3)) {   // Test for correct number of arguments
    cerr << "Usage: " << argv[0] 
         << " <Server> <Server Port>\n";
    exit(1);
  }

  string servAddress = argv[1];             // First arg: server address
  unsigned short servPort = Socket::resolveService(argv[2], "udp");

  try {
    UDPSocket sock;
    int jpegqual=95;
  
    Mat frame, send;
    vector<uchar> encoded;
    VideoCapture cap(0);//get the camera
    namedWindow("send",CV_WINDOW_AUTOSIZE);
    if(!cap.isOpened()){
      cerr << "OpenCV Failed to open camera";
      exit(1);
    }
    while(1){
      cap >> frame;
      resize(frame, send, Size(FRAME_WIDTH,FRAME_HEIGHT), 0, 0, INTER_LINEAR);
      vector<int> compression_params;
      compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
      compression_params.push_back(jpegqual);

      imencode(".jpg", send, encoded, compression_params);
      imshow("send", send);
      cout<<"sizeof encd:"<<encoded.size()<<endl;
      //if(encoded.size()>8190)//too big for udp! //shouldn't it bt 65535?
      //{
      //  cout<<"reducing jpeg quality..."<<jpegqual<<endl;
      //  jpegqual--;continue;
      //}
      //sock.sendTo(&encoded.front(), encoded.size(), servAddress, servPort);
      int total_pack=1+(encoded.size()-1)/PACK_SIZE;

      int  ibuf[1];ibuf[0]=total_pack;
      sock.sendTo(ibuf, sizeof(int), servAddress, servPort);

      for(int i=0;i<total_pack;i++)
        sock.sendTo(&encoded[i*PACK_SIZE], PACK_SIZE, servAddress, servPort);
      

      waitKey(FRAME_INTERVAL);
    }
    // Destructor closes the socket

  } catch (SocketException &e) {
    cerr << e.what() << endl;
    exit(1);
  }
  
  return 0;
}
