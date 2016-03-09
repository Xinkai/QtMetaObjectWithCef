#include <QDebug>
#include <QFile>
#include <QUrl>

#include "include/base/cef_logging.h"
#include "include/cef_scheme.h"

#include "SchemeHandlerFactory.h"

std::string ext2Mime(const std::string& fileName) {
    const auto pos = fileName.find_last_of('.');
    const auto ext = fileName.substr(pos + 1);

    if (ext == "css") {
        return "text/css";
    } else if (ext == "html") {
        return "text/html";
    } else if (ext == "js") {
        return "application/javascript";
    } else if (ext == "less") {
        return "text/less";
    } else if (ext == "svg") {
        return "image/svg+xml";
    } else if (ext == "png") {
       return "image/png";
    } else if (ext == "gif") {
        return "image/gif";
    } else if (ext == "jpg" || ext == "jpeg") {
        return "image/jpeg";
    } else {
        return "application/octet-stream";
    }
}

CefRefPtr<CefResourceHandler> SchemeHandlerFactory::Create(CefRefPtr<CefBrowser> browser,
                                                           CefRefPtr<CefFrame> frame,
                                                           const CefString& scheme_name,
                                                           CefRefPtr<CefRequest> request) {

    auto createQFileStreamResourceHandler = [](QString qrcPath) -> CefStreamResourceHandler* {
        if (!QFile::exists(qrcPath)) {
            return nullptr;
        }
        LOG(INFO) << "createQFileStreamResourceHandler " << qrcPath.toStdString();
        const auto mimeType = ext2Mime(QUrl(qrcPath).fileName().toStdString());

        QFile qFile(qrcPath);
        qFile.open(QIODevice::ReadOnly | QIODevice::Unbuffered);
        const auto content = qFile.readAll();

        const char* contentBytes = content.constData();
            CefRefPtr<CefStreamReader> stream =
                CefStreamReader::CreateForData(
                  static_cast<void*>(const_cast<char*>(contentBytes)),
                  (size_t)(qFile.size()));
        return new CefStreamResourceHandler(mimeType, stream);
    };

    if (scheme_name == "app") {
        const auto fullUrl = request->GetURL().ToString();
        const auto qUrl = QUrl(QString::fromStdString(fullUrl));

        if (qUrl.host() == "app") {
            return createQFileStreamResourceHandler(":/wwwroot" + qUrl.path());
        }
    }

    return nullptr;
}
