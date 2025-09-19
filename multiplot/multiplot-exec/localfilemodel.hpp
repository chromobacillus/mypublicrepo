#ifndef __5ade7c9a9ab9d5c6276e08cfb7960bc0e1a7fca9
#define __5ade7c9a9ab9d5c6276e08cfb7960bc0e1a7fca9

#include "multiplotdatamodel.hpp"

class LocalFileModel : public MultiplotDataModel {
Q_OBJECT
public:
    explicit LocalFileModel();
    ~LocalFileModel() override = default;

    ComtradeRecord record(const QString &filePath) override;
};

#endif //__5ade7c9a9ab9d5c6276e08cfb7960bc0e1a7fca9
