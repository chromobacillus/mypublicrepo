#include "localfilemodel.hpp"
#include "multiplot.hpp"

#include <QDebug>
#include <QTranslator>
#include <QtWidgets/QApplication>

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QTranslator translator;
    // qDebug() << "TRANSLATED:" << translator.load("/media/ilya/Data/job/portfolio/multiplot/build/Desktop-Release/multiplot-lib/multiplot-lib_ru.qm");
    // qDebug() << "TRANSLATED:" << translator.load(QLocale("ru"), "multiplot-lib_ru.qm", QString(),
    //         "/media/ilya/Data/job/portfolio/multiplot/build/Desktop-Release/multiplot-lib/");
    qDebug() << "SYSTEM LOCALE:" << QLocale::system();
    //if (QLocale::system().collation() != QLocale::English) {
        translator.load(QLocale::system(), "multiplot-lib_ru.qm");
    //}
    app.installTranslator(&translator);

    QSharedPointer<MultiplotDataModel> model(new LocalFileModel);
    Multiplot *multiplot = new Multiplot(model);
    multiplot->show();
    return app.exec();
}
