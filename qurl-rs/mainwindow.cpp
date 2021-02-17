#include "includes.h"
#include "rsnet.h"

MainWindow::MainWindow()
{
    setWindowTitle("qurl-rs");
    mainWidget = new QWidget();
    setCentralWidget(mainWidget);

    layout = new QGridLayout();

    buttonSend = new QPushButton("Send");
    QObject::connect(buttonSend, SIGNAL(clicked()), this, SLOT(sendRequest()));

    buttonClear = new QPushButton("Clear");
    QObject::connect(buttonClear, SIGNAL(clicked()), this, SLOT(clear()));

    target = new TargetBoxWidget();
    method = new MethodBoxWidget();
    agent = new UserAgentBoxWidget();
    headers = new HeadersBoxWidget();
    request = new RequestBoxWidget();
    response = new ResponseBoxWidget();

    layout->setRowStretch(3, 5);
    layout->setColumnStretch(2, 5);

    layout->addWidget(target, 0, 0, 1, 5);
    layout->addWidget(method, 1, 0, 1, 1);
    layout->addWidget(agent, 1, 2, 1, 3);
    layout->addWidget(headers, 2, 0, 1, 1);
    layout->addWidget(request->item(), 3, 0, 4, 2);
    layout->addWidget(response->item(), 3, 2, 3, 4);
    layout->addWidget(buttonClear, 6, 2, 1, 1);
    layout->addWidget(buttonSend, 6, 3, 1, 1);

    mainWidget->setLayout(layout);
    setMinimumSize(800,600);
}

std::string parse(std::string str)
{
    // Format the response so it looks like sorta palatable JSON
    std::string s = str;

    uint8_t level = 0;
    size_t size = s.size();
    bool noparse = false;
    for (unsigned int i = 0; i < size; i++)
    {
        // Check for quotation marks
        if ((s[i] == '\"') && (s[-1] != '\\'))
        {
            if (noparse == false) noparse = true;
            else if (noparse == true) noparse = false;
        }
        else if ((s[i] == '{') || (s[i] == '['))
        {
            if (noparse == false)
            {
                level+=2;
                s.insert(i+1, 1, '\n');
            }
        }
        else if ((s[i] == '}') || (s[i] == ']'))
        {
            if (noparse == false)
            {
                if (level - 1 >= 0) level-=2;

                char buf = s[i];
                s[i] = '\n';
                s.insert(i+1, level, ' ');
                s.insert(i+1+level, 1, buf);
                i = i+1+level;
            }
        }
        else if (s[i] == ',')
        {
            if (noparse == false) s.insert(i+1, 1, '\n');
        }
        else if (s[i] == '\n')
        {
            if (noparse == false)
            {
                s.insert(i+1, level, ' ');
            }
        }

        size = s.size();
    }

    return s;
}

void MainWindow::sendRequest()
{
    if (method->get() == "HEAD") headers->set(true);

    if (target->get().size() != 0)
    {
        Request r = ffi_request((char*)method->get().c_str(), (char*)target->get().c_str(), (char*)request->get().c_str(), (char*)agent->get().c_str());
        std::string _request = r.request;
        std::string _response = r.response;

        std::string _headers = "";
        std::string _body = "";
        if (_response != "Connection timed out")
        {
            _headers = _response.substr(0, _response.find("\r\n\r\n")+4);
            _body = _response.substr(_response.find("\r\n\r\n")+4);
        }

        // Are we looking at JSON data ?
        if ((_response.find("Content-Type: text/plain") != std::string::npos) || (_response.find("Content-Type: application/json") != std::string::npos))
        {
            _body = parse(_body);
        }

        std::stringstream ss;

        // Do we need the headers ?
        if (headers->get() == true)
        {
            ss << "Request: \n\n";

            if (method->get() == "HEAD")
            {
                ss << _request.substr(0, _request.find("\r\n\r\n"));
            }
            else
            {
                ss << _request.substr(0, _request.find("\r\n\r\n")+4);
                ss << parse(_request.substr(_request.find("\r\n\r\n")+4));

            }

            ss << "\n\nResponse:\n\n";
            ss << _headers;
        }

        // Do we need the body ?
        if (method->get() != "HEAD")
            ss << _body;

        response->set(ss.str());
    }
    else
    {
        response->set("Point me somewhere !");
    }
}

MainWindow::~MainWindow()
{

}

void MainWindow::clear()
{
    response->set("");
}
