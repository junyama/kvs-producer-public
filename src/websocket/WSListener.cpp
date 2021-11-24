
#include "WSListener.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WSListener

//constructor w/o a parameter
WSListener::WSListener() {}
//constructor with a parameter
WSListener::WSListener(MyGstreamer *gstreamerPtr_) {
    gstreamerPtr = gstreamerPtr_;
    OATPP_LOGD(TAG, "constructor with gstreamerPtr, %s", gstreamerPtr->returnMsg.c_str());
}

void WSListener::onPing(const WebSocket& socket, const oatpp::String& message) {
  OATPP_LOGD(TAG, "onPing");
  socket.sendPong(message);
}

void WSListener::onPong(const WebSocket& socket, const oatpp::String& message) {
  OATPP_LOGD(TAG, "onPong");
}

void WSListener::onClose(const WebSocket& socket, v_uint16 code, const oatpp::String& message) {
  OATPP_LOGD(TAG, "onClose code=%d", code);
}

void WSListener::readMessage(const WebSocket& socket, v_uint8 opcode, p_char8 data, oatpp::v_io_size size) {

  if(size == 0) { // message transfer finished

        auto wholeMessage = m_messageBuffer.toString();
        m_messageBuffer.clear();

        OATPP_LOGD(TAG, "onMessage message='%s'", wholeMessage->c_str());
      
      if (wholeMessage == "monitor"){
          /*
          if (monitoring) {
              OATPP_LOGD(TAG, "monitor already started.");
              socket.sendOneFrameText( "monitor already started!: " + wholeMessage);
          } else { */
              OATPP_LOGD(TAG, "monitor started.");
              socket.sendOneFrameText("gstreamer monitor started: " + wholeMessage);

              int returnCode;
              int count = 1;
              monitoring = true;
              do {
                  returnCode = gstreamerPtr->monitor();
                  std::string responseMsg = gstreamerPtr->monitorMsg;
                  OATPP_LOGD(TAG, "gstreamer(%d): %s\n", count, responseMsg.c_str());
                  socket.sendOneFrameText(("gstreamer(" + std::to_string(count) + "): " + responseMsg).c_str());
                  ++count;
                } while (returnCode != 0);
              monitoring = false;
        //  }
      } else {
          socket.sendOneFrameText( "not effective message!: " + wholeMessage);
      }
        
  } else if(size > 0) { // message frame received
    m_messageBuffer.writeSimple(data, size);
  }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WSInstanceListener

//constructor w/o a parameter
WSInstanceListener::WSInstanceListener() {}

//constructor with a parameter
WSInstanceListener::WSInstanceListener(MyGstreamer *gstreamerPtr_) {
    gstreamerPtr = gstreamerPtr_;
}

std::atomic<v_int32> WSInstanceListener::SOCKETS(0);

void WSInstanceListener::onAfterCreate(const oatpp::websocket::WebSocket& socket, const std::shared_ptr<const ParameterMap>& params) {

    SOCKETS ++;
    OATPP_LOGD(TAG, "New Incoming Connection. Connection count=%d", SOCKETS.load());

    /* In this particular case we create one WSListener per each connection */
    /* Which may be redundant in many cases */
    if (gstreamerPtr) 
        wsLinstener = std::make_shared<WSListener>(gstreamerPtr);
    else 
        wsLinstener = std::make_shared<WSListener>();
    socket.setListener(wsLinstener);
    //socket.setListener(std::make_shared<WSListener>());
}

void WSInstanceListener::onBeforeDestroy(const oatpp::websocket::WebSocket& socket) {

  SOCKETS --;
  OATPP_LOGD(TAG, "Connection closed. Connection count=%d", SOCKETS.load());

}