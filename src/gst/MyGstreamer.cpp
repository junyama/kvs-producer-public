#ifndef MyGstreamer_cpp
#define MyGstreamer_cpp

#define CAMERA_CONF_FILE_PATH "/home/xavier/github/kvs-producer/conf.d/gstreamer.json"

/*
#define STREAM_NAME "gst_test"
#define STORAGE_SIZE 512
#define ACCESS_KEY "AKIA2W5GL7UUMFHV54F4"
#define SECRET_KEY "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"
#define AWS_REGION "ap-south-1"

#define KVS_ADD_METADATA_G_STRUCT_NAME "kvs-add-metadata"
#define KVS_ADD_METADATA_NAME "name"
#define KVS_ADD_METADATA_VALUE "value"
#define KVS_ADD_METADATA_PERSISTENT "persist"
*/
#include <iostream>

#include <gst/gst.h>
#include "dto/DTOs.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"

class MyGstreamer {
private:
    constexpr static const char* TAG = "MyGstreamer";
    oatpp::Object<GstreamerConfigObj> gstreamerConfigObj;
    
    GstElement *pipeline, *source, *convert, *decoder, *encoder, /* *sink, */ *kvssink = nullptr;
    GstBus *bus = nullptr;
    GstMessage *msg = nullptr;
    GstStateChangeReturn ret;
    gboolean terminate = FALSE;
    GstCaps *caps, *caps2;
    gboolean playing;             /* Playing or Paused */
    GstEvent *event;
    
static gboolean link_elements_with_filter (GstElement *element1, GstElement *element2, GstCaps *caps) {
    gboolean link_ok;

    link_ok = gst_element_link_filtered (element1, element2, caps);
    gst_caps_unref (caps);

    if (!link_ok) {
    g_warning ("Failed to link element1 and element2!");
    }

    return link_ok;
}

    
public:
    std::string returnMsg = "Hello World!";
    std::string monitorMsg;
    
    MyGstreamer() {};
    MyGstreamer(oatpp::Object<GstreamerConfigObj> gstreamerConfigObj_) : gstreamerConfigObj(gstreamerConfigObj_){
        /*
        auto jsonStr = oatpp::String::loadFromFile(CAMERA_CONF_FILE_PATH);
        auto jsonObjectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
        if (jsonStr) {
                OATPP_LOGD(TAG, "Gstreamer configuration: %s", jsonStr->c_str());
                gstreamerConfigObj = jsonObjectMapper->readFromString<oatpp::Object<GstreamerConfigObj>>(jsonStr);
        } else OATPP_LOGD(TAG, "%s NOT existing", CAMERA_CONF_FILE_PATH);
        */
    }
    
    int start_kvs() {
        /* Initialize GStreamer */
        gst_init(0, {});

        /* Create the elements */
        source = gst_element_factory_make ("v4l2src", "source");
        convert = gst_element_factory_make ("videoconvert", "convert");
        encoder = gst_element_factory_make (gstreamerConfigObj->encoder->c_str(), "encoder");
        kvssink = gst_element_factory_make ("kvssink", "kvssink");

        /* Create the empty pipeline */
        pipeline = gst_pipeline_new ("test-pipeline");

        if (!pipeline || !source || !convert || !encoder || !kvssink) {
            g_printerr ("Not all elements could be created.\n");
            returnMsg = "Not all elements could be created.";
            return -1;
        }
        
        /* Build the pipeline */
        gst_bin_add_many (GST_BIN (pipeline), source, convert, encoder, /*decoder, sink,*/ kvssink, NULL);
        
        // link convert and encoder
        caps = gst_caps_new_simple ("video/x-raw",
          "format", G_TYPE_STRING, "I420",
          "width", G_TYPE_INT, *gstreamerConfigObj->width,
          "height", G_TYPE_INT, *gstreamerConfigObj->height,
          "framerate", GST_TYPE_FRACTION, *gstreamerConfigObj->FPS, 1,
          NULL);
        if (link_elements_with_filter (convert, encoder, caps) != true) {
            g_printerr ("convert and encoder with filter could not be linked.\n");
        }
        
        // link encoder and kvssink
        caps2 = gst_caps_new_simple ("video/x-h264",
          "stream-format", G_TYPE_STRING, "avc",
          "alignment", G_TYPE_STRING, "au",
          "profile", G_TYPE_STRING, "baseline",
          NULL);
        if (link_elements_with_filter (encoder, kvssink, caps2) != true) {
            g_printerr ("encoder and kvssink with filter could not be linked.\n");
        }
        
        if (gst_element_link (source, convert) != TRUE) {
            g_printerr ("source, convert could not be linked.\n");
            returnMsg = "source, convert could not be linked.";
            gst_object_unref (pipeline);
            return -1;
        }

        /* Modify Elements properties */
        g_object_set (source, "do-timestamp", gstreamerConfigObj->timeStamp, "device", gstreamerConfigObj->device->c_str(), NULL);
        //g_object_set (encoder, "bframes", 0, "key-int-max", 45, "bitrate", 500, NULL);
        g_object_set (encoder, "control-rate", 1, NULL);
        
        g_object_set (kvssink, "stream-name", gstreamerConfigObj->streamName->c_str(), "storage-size", *gstreamerConfigObj->StorageSize, "access-key", gstreamerConfigObj->AccessKey->c_str(), "secret-key", gstreamerConfigObj->secretKey->c_str(), "aws-region", gstreamerConfigObj->awsRegion->c_str(), NULL);

        // Start playing //
        g_print ("Start playing the pipeline......\n\n\n");
        ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
        if (ret == GST_STATE_CHANGE_FAILURE) {
            g_printerr ("Unable to set the pipeline to the playing state.\n");
            returnMsg = "Unable to set the pipeline to the playing state.";
            gst_object_unref (pipeline);
            return -1;
        }
        playing = TRUE;
        
        // Listen to the bus
        bus = gst_element_get_bus (pipeline);
        returnMsg = "pipleline has started playing";
        //
        return 0;
    }
    
    int pause() {
        playing = !playing;
        gst_element_set_state (pipeline, playing ? GST_STATE_PLAYING : GST_STATE_PAUSED);
        g_print ("Setting state to %s\n", playing ? "PLAYING" : "PAUSE");
        returnMsg = "pause operation executed.";
        return 0;
    }
    
    int stop_pipeline() {
        gst_element_set_state (pipeline, GST_STATE_NULL); //does not make state change message
        g_print ("Sat the state to NULL\n");
        gst_element_set_state (source, GST_STATE_NULL);
        returnMsg = "stop pipeline operation executed.";
        return 0;
    }
    
    int stop_kvssink() {
        g_print ("setting the kvssink state to NULL\n");
        gst_element_set_state (kvssink, GST_STATE_NULL);
        returnMsg = "stop kvssink operation executed.";
        return 0;
    }
    
    int send_metadata(std::string metadata_name, std::string metadata_value, bool persistent) {
        GstStructure *structure = gst_structure_new(gstreamerConfigObj->kvsAddMetadataStructureName->c_str(), 
                                                    gstreamerConfigObj->kvsAddMetadataName->c_str(), G_TYPE_STRING, metadata_name.c_str(),
                                                    gstreamerConfigObj->kvsAddMetadataValue->c_str(), G_TYPE_STRING, metadata_value.c_str(),
                                                    gstreamerConfigObj->kvsAddMetadataPersistent->c_str(), G_TYPE_BOOLEAN, persistent,
                                                    NULL);
        
        // Create a custom event
        event = gst_event_new_custom(GST_EVENT_CUSTOM_DOWNSTREAM , structure);
        
        // Send the event
        g_print ("calling gst_element_send_event()\n|\n|\n|\n|\n|\n|\n|\n");
        gst_element_send_event(kvssink, event);
        returnMsg = "send_metadata operation executed.";
        return 0;
    }
    
    int monitor() {
        terminate = false;
        monitorMsg = "";
        
        if (!bus) {
            monitorMsg = monitorMsg + "Pipeline already stoped.";
            return 0;
        }
        
        msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, static_cast<GstMessageType>(GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR | GST_MESSAGE_EOS)); //chages a line above for c++ more strict at enum casts

        // Parse message
        if (msg != NULL) {
          GError *err;
          gchar *debug_info;

          switch (GST_MESSAGE_TYPE (msg)) {
                case GST_MESSAGE_ERROR:
                    gst_message_parse_error (msg, &err, &debug_info);
                    g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
                    g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
                    monitorMsg = monitorMsg + "Error received from element. ";
                    g_clear_error (&err);
                    g_free (debug_info);
                    terminate = TRUE;
                    break;
                case GST_MESSAGE_EOS:
                    g_print ("End-Of-Stream reached.\n");
                    monitorMsg = monitorMsg + "End-Of-Stream reached. ";
                    terminate = TRUE;
                    break;
                case GST_MESSAGE_STATE_CHANGED:
                    GstState old_state, new_state, pending_state;
                    // We are only interested in state-changed messages from the pipeline, the source and the kvssink
                    if (GST_MESSAGE_SRC (msg) == GST_OBJECT (pipeline)) {
                        //GstState old_state, new_state, pending_state;
                        gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
                        g_print ("pipeline state changed from %s to %s:\n", gst_element_state_get_name (old_state), gst_element_state_get_name (new_state));
                        monitorMsg = monitorMsg + "pipeline state changed from ";
                        monitorMsg = monitorMsg + gst_element_state_get_name (old_state) + " to ";
                        monitorMsg = monitorMsg + gst_element_state_get_name (new_state) + ". ";
                        //if (new_state == GST_STATE_NULL) terminate = TRUE;
                    } else if (GST_MESSAGE_SRC (msg) == GST_OBJECT (source)){
                        gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
                        g_print ("source state changed from %s to %s:\n", gst_element_state_get_name (old_state), gst_element_state_get_name (new_state));
                        monitorMsg = monitorMsg + "source state changed from ";
                        monitorMsg = monitorMsg + gst_element_state_get_name (old_state) + " to ";
                        monitorMsg = monitorMsg + gst_element_state_get_name (new_state) + ". ";
                        if (new_state == GST_STATE_NULL) terminate = TRUE; //<=============================
                    } else if (GST_MESSAGE_SRC (msg) == GST_OBJECT (kvssink)){
                        gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
                        g_print ("kvssink state changed from %s to %s:\n", gst_element_state_get_name (old_state), gst_element_state_get_name (new_state));
                        monitorMsg = monitorMsg + "kvssink state changed from ";
                        monitorMsg = monitorMsg + gst_element_state_get_name (old_state) + " to ";
                        monitorMsg = monitorMsg + gst_element_state_get_name (new_state) + ". ";
                        if (new_state == GST_STATE_NULL) terminate = TRUE; //<=============================
                    } else {
                        gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
                        g_print ("element state changed from %s to %s:\n", gst_element_state_get_name (old_state), gst_element_state_get_name (new_state));
                        monitorMsg = monitorMsg + "element state changed from ";
                        monitorMsg = monitorMsg + gst_element_state_get_name (old_state) + " to ";
                        monitorMsg = monitorMsg + gst_element_state_get_name (new_state) + ". ";
                        if (new_state == GST_STATE_NULL) terminate = TRUE; //<=============================
                    }
                    break;
                default:
                    // We should not reach here
                    g_printerr ("Unexpected message received.\n");
                    monitorMsg = monitorMsg + "Unexpected message received. ";
                    break;
          }
          gst_message_unref (msg);
        } else {
            monitorMsg = monitorMsg + "No message received. ";
            terminate = TRUE; //<=============================
        }
        if (terminate) {
            //Free resources
            gst_object_unref (bus);
            gst_element_set_state (pipeline, GST_STATE_NULL);
            gst_object_unref (pipeline);
            
            monitorMsg = monitorMsg + "<< Monitoring terminated.";
            return 0;
        } else {
            monitorMsg = monitorMsg + ">> Monitoring contined.";
            return 1;
        }
    }
        
};

#endif