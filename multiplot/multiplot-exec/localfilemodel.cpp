#include "localfilemodel.hpp"
#include "src/simleoscreader.hpp"
#include "comtraderecord.hpp"

LocalFileModel::LocalFileModel() {
}

ComtradeRecord LocalFileModel::record(const QString &filePath) {
    SimleOscReader reader(this);
    reader.readComtradeRecord(filePath);
    return reader.comtradeRecord();
}
