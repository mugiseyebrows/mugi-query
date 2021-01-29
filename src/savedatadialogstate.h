#ifndef SAVEDATADIALOGSTATE_H
#define SAVEDATADIALOGSTATE_H
#include <QString>

class SaveDataDialogState {
public:
    SaveDataDialogState();
    SaveDataDialogState(const QString& outputType, const QString& dataFormat,
                        const QString& outputDir);
    QString outputType() const;
    void setOutputType(const QString& outputType);
    QString dataFormat() const;
    void setDataFormat(const QString& dataFormat);
    QString outputDir() const;
    void setOutputDir(const QString& outputDir);
    bool isValid() const;

    void setOutputType(int type);
    void setDataFormat(int format);
protected:
    QString mOutputType;
    QString mDataFormat;
    QString mOutputDir;
};
#endif // SAVEDATADIALOGSTATE_H
