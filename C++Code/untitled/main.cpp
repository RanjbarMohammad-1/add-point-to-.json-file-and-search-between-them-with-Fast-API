#include <QCoreApplication>
#include<QDebug>

#include "FastAPI.h"

#include <curl/curl.h>
#include <iostream>
#include <stdexcept>

using namespace std;
using json = nlohmann::json;

using namespace std;
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug("hiiiiiiiiiiii");


///////////////////////////////
///////////////////////////////
///////////////////////////////

   // Initialize CURL
    curl_global_init(CURL_GLOBAL_DEFAULT);

    try
    {
        FastAPI api;

        // --------------------------------
        // 1. Add a new geographical point
        // --------------------------------

        QString newPointName = "تبریز";

        bool added = api.addPoint(
            newPointName.toStdString(),
            32.6546,
            51.6680
        );

        if (added)
        {
            qDebug() << "New point added successfully!";
        }
        else
        {
            qDebug() << "Point already exists.";
        }

        // --------------------------------
        // 2. Search for another point
        // --------------------------------

        QString searchName = "اصفهان";

        auto result = api.searchPoint(
            searchName.toStdString()
        );

        if (result.has_value())
        {
            json point = result.value();

            qDebug() << "Point found!";
            qDebug() << "Name:"
                     << QString::fromStdString(
                            point["name"].get<string>()
                        );

            qDebug() << "Latitude:"
                     << point["latitude"].get<double>();

            qDebug() << "Longitude:"
                     << point["longitude"].get<double>();
        }
        else
        {
            qDebug() << "Point not found!";
        }
    }
    catch (const exception& error)
    {
        qDebug() << "Error:"
                 << error.what();
    }

    // Clean up CURL
    curl_global_cleanup();

//////////////////////////////
//////////////////////////////
//////////////////////////////



    return a.exec();
}
