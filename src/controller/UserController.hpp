#ifndef UserController_hpp
#define UserController_hpp

#define GSTREAMER_CONF_FILE_PATH "/home/xavier/github/kvs-producer/conf.d/gstreamer.json"

//#include "service/UserService.hpp"

#include "dto/DTOs.hpp"
#include "gst/MyGstreamer.cpp"
#include "websocket/WSListener.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include "oatpp-websocket/Handshaker.hpp"
#include "oatpp/network/ConnectionHandler.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController) //<- Begin Codegen

/**
 * User REST controller.
 */
class UserController : public oatpp::web::server::api::ApiController {
public:
    UserController(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
    : oatpp::web::server::api::ApiController(objectMapper)
    {
        auto jsonStr = oatpp::String::loadFromFile(GSTREAMER_CONF_FILE_PATH);
        auto jsonObjectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
        if (jsonStr) {
                OATPP_LOGD(TAG, "Gstreamer configuration: %s", jsonStr->c_str());
                gstreamerConfigObj = jsonObjectMapper->readFromString<oatpp::Object<GstreamerConfigObj>>(jsonStr);
        } else OATPP_LOGD(TAG, "%s NOT existing", GSTREAMER_CONF_FILE_PATH);
        myGstreamer = MyGstreamer(gstreamerConfigObj);
    }
private:
    constexpr static const char* TAG = "UserController";
    //UserService m_userService; // Create user service.

    oatpp::Object<GstreamerConfigObj> gstreamerConfigObj;
    
    MyGstreamer myGstreamer;

    std::shared_ptr<oatpp::websocket::ConnectionHandler> createWebsocketConnectionHandler (MyGstreamer *gstreamerPtr) {
        auto connectionHandler = oatpp::websocket::ConnectionHandler::createShared();
        connectionHandler->setSocketInstanceListener(std::make_shared<WSInstanceListener>(gstreamerPtr));
        return connectionHandler;
    }
    
public:
    static std::shared_ptr<UserController> createShared(
    OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper) // Inject objectMapper component here as default parameter
    ){
    return std::make_shared<UserController>(objectMapper);
    }
    
    ENDPOINT_INFO(start) {
      info->summary = "start pipeline";
      info->addResponse<Object<StatusDto>>(Status::CODE_200, "application/json");
    }

    ENDPOINT("GET", "/start", start) { 
        auto dto = StatusDto::createShared();
        dto->returnCode = myGstreamer.start_kvs();
        dto->statusCode = 200;
        dto->message = myGstreamer.returnMsg.c_str();
        return createDtoResponse(Status::CODE_200, dto);
    }

    ENDPOINT_INFO(pause) {
      info->summary = "pause pipeline";

      info->addResponse<Object<StatusDto>>(Status::CODE_200, "application/json");
    }

    ENDPOINT("GET", "/pause", pause) { 
        auto dto = StatusDto::createShared();
        dto->returnCode = myGstreamer.pause();
        dto->statusCode = 200;
        dto->message = myGstreamer.returnMsg.c_str();
        return createDtoResponse(Status::CODE_200, dto);
    }

    ENDPOINT_INFO(stop_pipeline) {
      info->summary = "stop pipeline";

      info->addResponse<Object<StatusDto>>(Status::CODE_200, "application/json");
    }

    ENDPOINT("GET", "/stop_pipeline", stop_pipeline) {
        auto dto = StatusDto::createShared();
        dto->returnCode = myGstreamer.stop_pipeline();
        dto->statusCode = 200;
        dto->message = myGstreamer.returnMsg.c_str();
        return createDtoResponse(Status::CODE_200, dto);
    }

    ENDPOINT_INFO(stop_kvssink) {
      info->summary = "stop kvssink";

      info->addResponse<Object<StatusDto>>(Status::CODE_200, "application/json");
    }

    ENDPOINT("GET", "/stop_kvssink", stop_kvssink) {
        auto dto = StatusDto::createShared();
        dto->returnCode = myGstreamer.stop_kvssink();
        dto->statusCode = 200;
        dto->message = myGstreamer.returnMsg.c_str();
        return createDtoResponse(Status::CODE_200, dto);
    }

    ENDPOINT_INFO(send_metadata) {
        info->summary = "send metadata event to kvssink";

        info->addConsumes<String>("plain/text");

        info->addResponse<Object<StatusDto>>(Status::CODE_200, "application/json");
    }

    ENDPOINT("POST", "/send_metadata", send_metadata, BODY_STRING(String, jsonStr)) {
        std::string metadata_value = jsonStr;
        auto dto = StatusDto::createShared();
        dto->returnCode = myGstreamer.send_metadata("location", metadata_value, true);
        dto->statusCode = 200;
        dto->message = myGstreamer.returnMsg.c_str();
        return createDtoResponse(Status::CODE_200, dto);
    }

    ENDPOINT("GET", "/ws", ws, REQUEST(std::shared_ptr<IncomingRequest>, request)) {
        return oatpp::websocket::Handshaker::serversideHandshake(request->getHeaders(), createWebsocketConnectionHandler(&myGstreamer));
    }
    
    ENDPOINT_INFO(getSwaggerIp) {
        info->summary = "Get Swagger IP address";
        info->addResponse<String>(Status::CODE_200, "text/plain");
        info->addResponse<Object<StatusDto>>(Status::CODE_400, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_401, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");
    }

    ENDPOINT("GET", "/getSwaggerIp", getSwaggerIp) {
        oatpp::String returnText = getenv("SWAGGER_SERVER_IP");;
        if (returnText == nullptr) {
            OATPP_LOGD(TAG, "SWAGGER_SERVER_IP not defined");
            returnText = "192.168.0.142";
        }
        OATPP_LOGD(TAG, "SWAGGER_SERVER_IP: %s", returnText->c_str());
        auto response = createResponse(Status::CODE_200, returnText);
        response->putHeader(Header::CONTENT_TYPE, "text/plain");
        return response;
    }

    ENDPOINT_INFO(getGstreamerConfig) {
        info->summary = "Get configuration of Gstreamer";
        info->addResponse<Object<GstreamerConfigObj>>(Status::CODE_200, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_400, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_401, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");
    }

    ENDPOINT("GET", "/getGstreamerConfig", getGstreamerConfig) {
        auto response = createDtoResponse(Status::CODE_200, gstreamerConfigObj);
        return response;
    }
    
    ENDPOINT_INFO(loadGstreamerConfig) {
        info->summary = "Load configuration of Gstreamer";
        info->addResponse<Object<GstreamerConfigObj>>(Status::CODE_200, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_400, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_401, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");
    }

    ENDPOINT("POST", "/loadGstreamerConfig", loadGstreamerConfig, BODY_DTO(Object<GstreamerConfigObj>, gstreamerConfigObj_)) {
        gstreamerConfigObj = gstreamerConfigObj_;
        auto response = createDtoResponse(Status::CODE_200, gstreamerConfigObj);
        return response;
    }
    
    ENDPOINT_INFO(saveGstreamerConfig) {
        info->summary = "Save configuration of Gstreamer";
        info->addResponse<String>(Status::CODE_200, "text/plain");
        info->addResponse<Object<StatusDto>>(Status::CODE_400, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_401, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");
    }

    ENDPOINT("GET", "/saveGstreamerConfig", saveGstreamerConfig) {
        auto serializerConfig = oatpp::parser::json::mapping::Serializer::Config::createShared();
        serializerConfig->useBeautifier = true;
        serializerConfig->escapeFlags = 0;
        auto deserializerConfig = oatpp::parser::json::mapping::Deserializer::Config::createShared();
        auto jsonObjectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared(serializerConfig, deserializerConfig);
        oatpp::String jsonStr = jsonObjectMapper->writeToString(gstreamerConfigObj); 
        std::string filePath = GSTREAMER_CONF_FILE_PATH;
        jsonStr.saveToFile(filePath.c_str());
        OATPP_LOGD(TAG, "Configuration of Gstreamer is saved at %s", filePath.c_str());
        const char* text = "OK";
        auto response = createResponse(Status::CODE_200, text);
        response->putHeader(Header::CONTENT_TYPE, "text/plane");
        return response;
    }
    
};

#include OATPP_CODEGEN_END(ApiController) //<- End Codegen

#endif /* UserController_hpp */
