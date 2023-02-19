#include "WebContentHandler.h"
#include <dirent.h>
    
WebContentHandler::WebContentHandler() : HttpUrlHandler(GetWebFiles(), HTTP_GET )  {

}

WebContentHandler::~WebContentHandler() {

}
bool hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

std::vector<const char *> WebContentHandler::_files;
std::vector<const char *> WebContentHandler::_webNames;
bool WebContentHandler::_loaded  = false;
void WebContentHandler::ProcessRequest(HttpRequest *pReq) {
    printf("Uri: %s\n", pReq->GetUri().c_str());
    std::string gzUrl = pReq->GetUri()+".gz";
    std::string brUrl = pReq->GetUri()+".br";   
    std::string bzIndex = pReq->GetUri()+"index.html.gz";

    for(auto file : _files) {
        printf("File: %s\n", file);
        const int SizeToRead = 1024*16;
        if(pReq->GetUri()==file || bzIndex.compare(file)==0 || gzUrl.compare(file)==0 || brUrl.compare(file)==0) {
            std::string fn = file==std::string("/") ? std::string("/web/index.html") : std::string("/web")+file;
             printf("FN: %s\n", fn.c_str());
          
            auto fp = fopen(fn.c_str(),"r");
            if(!fp) {
                printf("failed to openfile\n");
                return;
            }

            fseek(fp, 0L, SEEK_END);
            auto sz = ftell(fp);
            printf("File is %lu bytes\n", sz);
            rewind(fp);
            char* buf = (char*)malloc(SizeToRead);
            if(hasEnding(fn,".html")||hasEnding(fn,".htm")) {
                pReq->SetType("text/html;charset=UTF-8");
            } else if(hasEnding(fn, ".js")) {
                pReq->SetType("application/javascript;charset=UTF-8");
            } else if(hasEnding(fn, ".png")) {
                pReq->SetType("image/png");
            } else if(hasEnding(fn,".html.br")||hasEnding(fn,".htm.br")) {
                pReq->SetType("text/html;charset=UTF-8");
                pReq->SetHeader("Content-Encoding", "br");
            } else if(hasEnding(fn, ".js.br")) {
                pReq->SetType("application/javascript;charset=UTF-8");
                pReq->SetHeader("Content-Encoding", "br");
            } else if(hasEnding(fn, ".png.br")) {
                pReq->SetType("image/png.br");
                pReq->SetHeader("Content-Encoding", "br");
            } else if(hasEnding(fn,".html.gz")||hasEnding(fn,".htm.gz")) {
                pReq->SetType("text/html;charset=UTF-8");
                pReq->SetHeader("Content-Encoding", "gzip");                                
            } else if(hasEnding(fn, ".js.gz")) {
                pReq->SetType("application/javascript;charset=UTF-8");
                pReq->SetHeader("Content-Encoding", "gzip");
            } else if(hasEnding(fn, ".png.gz")) {                
                pReq->SetType("image/png.gz");
                pReq->SetHeader("Content-Encoding", "gzip");                
            }
            while(sz>0) {
                auto read = fread(buf,1,sz<SizeToRead? sz : SizeToRead,fp);
                if(read>0) {
                    pReq->SendChunk(buf, read);
                   
                }
                sz-=read;
                if(read==0 ) break;
            }

            fclose(fp);
            free(buf);
            pReq->FinishedChunks();
            return;
        }
    }
    
    printf("Got content request: %s\n", pReq->GetUri().c_str());
    return;
}

std::vector<const char *> WebContentHandler::GetWebFiles() {
    if(!_loaded) {
        DIR *d;
        struct dirent *dir;
        d = opendir("/web");
        if (d) {
            _webNames.push_back("/");
            while ((dir = readdir(d)) != NULL) {
                char *path = (char *)calloc(strlen(dir->d_name)+2,1);
                path[0] = '/';
                memcpy(path+1, dir->d_name,strlen(dir->d_name));
 
                _files.push_back((const char*)path);
                if(hasEnding(path, ".gz") || hasEnding(path,".br")) {
                    char *webVisiblePath = (char *)calloc(strlen(path)-2,1);
                    memcpy(webVisiblePath, path,strlen(path)-3);
                    _webNames.push_back(webVisiblePath);
                } else
                    _webNames.push_back(path);
            }
            closedir(d);
        }

        _loaded = true;
    }
    return _webNames;
}