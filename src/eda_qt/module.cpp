#include "module.h"
#include <QDebug>
#include <QFile>
#include <QtXml>
Module::Module()
{

}

Module::Module(QString name,int inputPorts,int outputPorts,int inOutPorts)
{
    this->name = name;
    this->latestNum = 0;
    for(int i = 0;i<inputPorts;i++,this->latestNum++)
    {
        this->ports.push_back(Port("p"+QString::number(this->latestNum),INPUT,0,1,this->latestNum));
    }
    for(int i = 0;i<outputPorts;i++,this->latestNum++)
    {
        this->ports.push_back(Port("p"+QString::number(this->latestNum),OUTPUT,0,1,this->latestNum));
    }
    for(int i = 0;i<inOutPorts;i++,this->latestNum++)
    {
        this->ports.push_back(Port("p"+QString::number(this->latestNum),INOUT,0,1,this->latestNum));
    }
}

Port & Module::getSelectedPort(int portNum)
{
    for(unsigned long i = 0;i<this->ports.size();i++){
        if(this->ports.at(i).getPortNum()==portNum)
            return this->ports.at(i);
    }
}

void Module::addPort()
{
    this->ports.push_back(Port("p"+QString::number(this->latestNum),INPUT,0,1,this->latestNum));
}
void Module::deletePort(int portNum)
{

    if(this->ports[portNum].getPortType()==INPUT){
       this->inputPorts--;
    }else if(this->ports[portNum].getPortType()==OUTPUT) {
       this->outputPorts--;
    }else{
        this->inOutPorts--;
    }
    for(unsigned long i = 0;i<this->ports.size();i++){
        if(this->ports.at(i).getPortNum()==portNum)
            this->ports.erase(this->ports.begin()+i);
    }
}

void Module::setCode(QString code)
{
    this->code = code;
}

QString Module::getCode()
{
    return this->code;
}

void Module::setName(QString name)
{
    this->name = name;
}

QString Module::getName()
{
    return this->name;
}

void Module::setAnnotation(QString annotation)
{
    this->annotation = annotation;
}

QString Module::getAnnotation()
{
    return this->annotation;
}

QString Module::generateCode(QString code)
{
    QString generateCodes;
    generateCodes = "module "+this->name+"(\n";
    for(unsigned long i = 0;i<this->ports.size();i++){
        if(i ==this->ports.size()-1){
            generateCodes =generateCodes + "\t" + this->ports.at(i).getName();
        }else{
            generateCodes =generateCodes + "\t" + this->ports.at(i).getName()+",\n";
        }
    }
    generateCodes = generateCodes+");\n\n";
    for(unsigned long i = 0;i<this->ports.size();i++){
            if(this->ports.at(i).getPortType()==INPUT){
                generateCodes =generateCodes + "\tinput ";

            }else if (this->ports.at(i).getPortType()==OUTPUT) {
                generateCodes =generateCodes + "\toutput ";
            }else{
                generateCodes =generateCodes + "\tinout ";
            }
            if(this->ports.at(i).getDataSize()==1){//??????port???DataSize????????????1.
                generateCodes =generateCodes + this->ports.at(i).getName()+";\n";
            }else{
                generateCodes =generateCodes + " [" + QString::number(this->ports.at(i).getDataSize()-1) + ":0] " + this->ports.at(i).getName()+";\n";
            }
    }
    generateCodes = generateCodes+"\n";
    for(unsigned long i = 0;i<this->ports.size();i++){
            if(this->ports.at(i).getDataType()==0){
                generateCodes =generateCodes + "\twire ";

            }else{
                generateCodes =generateCodes + "\treg ";
            }
            if(this->ports.at(i).getDataSize()==1){//??????port???DataSize????????????1.
                generateCodes =generateCodes + this->ports.at(i).getName()+";\n";
            }else{
                generateCodes =generateCodes + " [" + QString::number(this->ports.at(i).getDataSize()-1) + ":0] " + this->ports.at(i).getName()+";\n";
            }
    }
    generateCodes = generateCodes + "\n" +code+"\n\nendmodule";
    return generateCodes;
}
QString Module::saveCodeFile(QString geneCodes)
{
    QString fileName = QFileDialog::getSaveFileName(NULL, QStringLiteral("??????Verilog????????????"),this->getName(),QStringLiteral("verilog(*.v)"));
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly)){
        return("???????????????????????????????????????\n");
    }
    QByteArray geneCodesArr = geneCodes.toUtf8();//???qstring?????????qbytearray
    file.write(geneCodesArr);
    file.close();
    return ("????????????????????????????????? "+fileName+"\n");
}
QDomDocument Module::module_relay()
{
    QDomDocument doc;
    QDomProcessingInstruction instruction;  //??????????????????????????????
    QString data;
    data = "version=\"1.0\" encoding=\"UTF-8\"";
    instruction = doc.createProcessingInstruction("xml", data);
    doc.appendChild(instruction);
    QDomElement elementRoot = doc.createElement("module");
    QDomElement element_M_Name = doc.createElement("m_name");
    QDomElement element_M_Annotation = doc.createElement("m_annotation");
    QDomElement element_Code = doc.createElement("code");
    QDomElement element_Ports = doc.createElement("Ports");
    for(unsigned long i = 0;i<this->ports.size();i++){  //?????????????????????????????????
        QDomElement element_Port = doc.createElement(this->ports.at(i).getName());//??????????????????????????????
        element_Ports.appendChild(element_Port);//?????????????????????ports??????
        QDomElement element_Port_PType = doc.createElement("PType");//????????????????????????????????????
            if(this->ports.at(i).getPortType()==INPUT){
                element_Port_PType.appendChild(doc.createTextNode("INPUT"));
            }else if(this->ports.at(i).getPortType()==OUTPUT){
                element_Port_PType.appendChild(doc.createTextNode("OUTPUT"));
            }else{
                element_Port_PType.appendChild(doc.createTextNode("INOUT"));
            }
        element_Port.appendChild(element_Port_PType);//???????????????????????????????????????
        QDomElement element_Port_DType = doc.createElement("DType");//??????????????????????????????
        element_Port_DType.appendChild(doc.createTextNode(this->ports.at(i).getDataType()?"reg":"wire"));
        element_Port.appendChild(element_Port_DType);
        QDomElement element_Port_DSize = doc.createElement("DSize");
        element_Port_DSize.appendChild(doc.createTextNode(QString::number(this->ports.at(i).getDataSize())));
        element_Port.appendChild(element_Port_DSize);
        QDomElement element_Port_Annotation = doc.createElement("Annotation");
        element_Port_Annotation.appendChild(doc.createTextNode(this->ports.at(i).getAnnotation()));
        element_Port.appendChild(element_Port_Annotation);
    }

    element_M_Name.appendChild(doc.createTextNode(this->name));
    element_M_Annotation.appendChild(doc.createTextNode(this->getAnnotation()));
    element_Code.appendChild(doc.createTextNode(this->getCode()));
    elementRoot.appendChild(element_M_Name);
    elementRoot.appendChild(element_M_Annotation);
    elementRoot.appendChild(element_Code);
    elementRoot.appendChild(element_Ports);
    doc.appendChild(elementRoot);
    return doc;

}
//QString Module::saveModuleFile()
//{

//    QString fileName = QFileDialog::getSaveFileName(NULL, QStringLiteral("??????Module??????"),this->getName(),QStringLiteral("Module(*.mod)"));
//    QFile file(fileName);
//    if(!file.open(QIODevice::WriteOnly)){
//        return("???????????????????????????????????????\n");
//    }
//    file.close();

//    QDomDocument doc;

//    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
//        {
//            qDebug() << "open for add error!!";
//        }
//    doc = Module::module_relay();
//    QTextStream out(&file);
//    doc.save(out, 4);
//    file.close();
//    return ("????????????????????????????????? "+fileName+"\n");

//}
