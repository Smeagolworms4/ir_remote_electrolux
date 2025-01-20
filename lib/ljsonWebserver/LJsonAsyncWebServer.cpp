#include <LJsonAsyncWebServer.h>

LJsonAsyncWebServer::LJsonAsyncWebServer(uint16_t port): AsyncWebServer(port) {}

AsyncCallbackWebHandler& LJsonAsyncWebServer::onBody(const char* uri, WebRequestMethodComposite method, ArGroupedBodyHandlerFunction onBody) {

    return this->on(
        uri,
        method,
        [this, onBody](AsyncWebServerRequest* request){
            String body = this->body.c_str();
            this->body.clear();
            onBody(request, body);
        },
        [](AsyncWebServerRequest* request, const String& filename, size_t index, uint8_t* data, size_t len, bool final) {},
        [this](AsyncWebServerRequest* request, uint8_t *data, size_t len, size_t index, size_t total) {
            if (index == 0) {
                this->body = "";
                this->body.reserve(total);
            }
            for (size_t i = 0; i < len; i++) this->body += (char)data[i];
        }
    );
}

AsyncCallbackWebHandler& LJsonAsyncWebServer::onUpload(const char* uri, WebRequestMethodComposite method, ArGroupedUploadHandlerFunction onUpload) {
    return this->on(
        uri,
        method,
        [this, onUpload](AsyncWebServerRequest* request){
            WebUpload file = {
                filename = this->filename,
                this->upload.c_str()
            };
            this->upload = "";

            onUpload(request, file);
        },
        [this](AsyncWebServerRequest* request, const String& filename, size_t index, uint8_t* data, size_t len, bool final) {
            if (index == 0) {
                this->upload = "";
            }
            this->upload.reserve(index + len);
            for (size_t i = 0; i < len; i++) this->upload += (char)data[i];
        }
    );
}

AsyncCallbackWebHandler& LJsonAsyncWebServer::onBodyAndUpload(const char* uri, WebRequestMethodComposite method, ArGroupedBodyAndUploadHandlerFunction onBodyAndUpload) {
    return this->on(
        uri,
        method,
        [this, onBodyAndUpload](AsyncWebServerRequest* request){
            String body = this->body.c_str();
            WebUpload file = {
                filename = this->filename,
                this->upload.c_str()
            };
            this->upload = "";

            onBodyAndUpload(request, body, file);
        },
        [this](AsyncWebServerRequest* request, const String& filename, size_t index, uint8_t* data, size_t len, bool final) {
            if (index == 0) {
                this->upload = "";
            }
            this->upload.reserve(index + len);
            for (size_t i = 0; i < len; i++) this->upload += (char)data[i];
        },
        [this](AsyncWebServerRequest* request, uint8_t *data, size_t len, size_t index, size_t total) {
            if (index == 0) {
                this->body = "";
                this->body.reserve(total);
            }
            for (size_t i = 0; i < len; i++) this->body += (char)data[i];
        }
    );
}

AsyncCallbackWebHandler& LJsonAsyncWebServer::onJson(const char* uri, WebRequestMethodComposite method, std::function<void(AsyncWebServerRequest* request, LJsonNode* json)> callback) {
    return this->onJson(uri, method, [callback](AsyncWebServerRequest* request, LJsonNode* json, String body) {
        callback(request, json);
    });
}

AsyncCallbackWebHandler& LJsonAsyncWebServer::onJson(const char* uri, WebRequestMethodComposite method, std::function<void(AsyncWebServerRequest* request, LJsonNode* json, String body)> callback) {
    return this->onBody(uri, method, [callback](AsyncWebServerRequest* request, String body) {
        LJsonNode* json = ljson_parse(body);
        if (json) {
            callback(request, json, body);
            return;
        }
        request->send(400, "application/json", "{\"error\":\"Parsing error\"}");
    });
    
}

String generate_random_filename() {
    unsigned long time = millis();
    int random_number = random(1000, 9999);  // Generate a random number between 1000 and 9999
    return "/_tmp_res_" + String(time) + "_" + String(random_number) + ".json";
}

void send_json_response(AsyncWebServerRequest* request, int code, LJsonNode* node) {
    send_json_response(request, code, node, false);
}

void send_json_response(AsyncWebServerRequest* request, int code, LJsonNode* node, bool clean) {
    String filename = generate_random_filename();
    File file = LittleFS.open(filename, "w");
    write_json_to_file(file, node);
    file.close();
    if (clean) {
        delete node;
    }

    AsyncWebServerResponse *response = request->beginResponse(LittleFS, filename, "application/json");
    response->setCode(code);
    request->send(response);    
    
    LittleFS.remove(filename);
}

void clean_tmp_res_file() {
    Dir dir = LittleFS.openDir("/");
    while (dir.next()) {
        String filename = dir.fileName();
        if (filename.startsWith("/_tmp_res_")) {
            if (LittleFS.remove(filename)) {
                Serial.println("Deleted file: " + filename);
            } else {
                Serial.println("Failed to delete file: " + filename);
            }
        }
    }
}