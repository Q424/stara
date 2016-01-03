/*
This Source Code Form is subject to the
terms of the Mozilla Public License, v.
2.0. If a copy of the MPL was not
distributed with this file, You can
obtain one at
http://mozilla.org/MPL/2.0/.
*/

#define _USE_OLD_RW_STL

#include "parser.h"
#include "logs.h"
#include "globals.h"
/*
    MaSzyna EU07 locomotive simulator parser
    Copyright (C) 2003  TOLARIS

*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
// cParser -- generic class for parsing text data.

// constructors
cParser::cParser(std::string Stream, buffertype Type, std::string Path, bool tr)
{
    LoadTraction = tr;
    // build comments map
    mComments.insert(commentmap::value_type("/*", "*/"));
    mComments.insert(commentmap::value_type("//", "\n"));
    // mComments.insert(commentmap::value_type("--","\n")); //Ra: to chyba nie u�ywane
    // store to calculate sub-sequent includes from relative path
    mPath = Path;
    // reset pointers and attach proper type of buffer
    switch (Type)
    {
    case buffer_FILE:
        Path.append(Stream);
        mStream = new std::ifstream(Path.c_str());
        break;
    case buffer_TEXT:
        mStream = new std::istringstream(Stream);
        break;
    default:
        mStream = NULL;
    }
    mIncludeParser = NULL;
    // calculate stream size
    if (mStream)
    {
        mSize = mStream->rdbuf()->pubseekoff(0, std::ios_base::end);
        mStream->rdbuf()->pubseekoff(0, std::ios_base::beg);
    }
    else
        mSize = 0;
}

// destructor
cParser::~cParser()
{
    if (mIncludeParser)
        delete mIncludeParser;
    if (mStream)
        delete mStream;
    mComments.clear();
}

// methods
bool cParser::getTokens(int Count, bool ToLower, const char *Break)
{
    /*
     if (LoadTraction==true)
      trtest="niemaproblema"; //wczytywa�
     else
      trtest="x"; //nie wczytywa�
    */
    int i;
    this->str("");
    this->clear();
    for (i = 0; i < Count; ++i)
    {
        std::string string = readToken(ToLower, Break);
        // collect parameters
        if (i == 0)
            this->str(string);
        else
        {
            std::string temp = this->str();
            temp.append("\n");
            temp.append(string);
            this->str(temp);
        }
    }
    if (i < Count)
        return false;
    else
        return true;
}

std::string cParser::readToken(bool ToLower, const char *Break)
{
    AnsiString par, tst, trainnumber, stationname;
    std::string token = "";
    size_t pos; // pocz�tek podmienianego ci�gu
    // see if there's include parsing going on. clean up when it's done.
    if (mIncludeParser)
    {
        token = (*mIncludeParser).readToken(ToLower, Break);
        if (!token.empty())
        {
            pos = token.find("(p");
            // check if the token is a parameter which should be replaced with stored true value
            if (pos != std::string::npos) //!=npos to znalezione
            {
                std::string parameter =
                    token.substr(pos + 2, token.find(")", pos) - pos + 2); // numer parametru
                token.erase(pos, token.find(")", pos) - pos + 1); // najpierw usuni�cie "(pN)"
                size_t nr = atoi(parameter.c_str()) - 1;
                if (nr < parameters.size())
                {
                    token.insert(pos, parameters.at(nr)); // wklejenie warto�ci parametru
                    if (ToLower)
                        for (; pos < token.length(); ++pos)
                            token[pos] = tolower(token[pos]);
                }
                else
                    token.insert(pos, "none"); // zabezpieczenie przed brakiem parametru
            }
            return token;
        }
        else
        {
            delete mIncludeParser;
            mIncludeParser = NULL;
            parameters.clear();
        }
    }
    // get the token yourself if there's no child to delegate it to.
    char c;
    do
    {
        while (mStream->peek() != EOF && strchr(Break, c = mStream->get()) == NULL)
        {
            if (ToLower)
                c = tolower(c);
            token += c;
            if (trimComments(token)) // don't glue together words separated with comment
                break;
        }
    } while (token == "" && mStream->peek() != EOF); // double check to deal with trailing spaces
    // launch child parser if include directive found.
    // NOTE: parameter collecting uses default set of token separators.
    if (token.compare("include") == 0)
    { // obs�uga include
        std::string includefile = readToken(ToLower); // nazwa pliku
        
        QGlobal::asINCLUDEFILE = AnsiString(includefile.c_str());

        if (LoadTraction ? true : ((includefile.find("tr/") == std::string::npos) &&
                                   (includefile.find("tra/") == std::string::npos)))
        {
            // std::string trtest2="niemaproblema"; //nazwa odporna na znalezienie "tr/"
            // if (trtest=="x") //je�li nie wczytywa� drut�w
            // trtest2=includefile; //kopiowanie �cie�ki do pliku
            std::string parameter = readToken(false); // w parametrach nie zmniejszamy
            while (parameter.compare("end") != 0)
            {
                parameters.push_back(parameter);
                parameter = readToken(ToLower);
            }
            // if (trtest2.find("tr/")!=0)
            mIncludeParser = new cParser(includefile, buffer_FILE, mPath, LoadTraction);
            if (mIncludeParser->mSize <= 0)
                ErrorLog("Missed include: " + AnsiString(includefile.c_str()));
        }
        else
            while (token.compare("end") != 0)
                token = readToken(ToLower);
        token = readToken(ToLower, Break);
    }


// **********************************************************************************************************
// Q: 24.12.15 - Dodaje nowy znacznik wpisu include potrzebny do wprowadzenia includeow z nowym podstawowym
//               parametrem jakim bedzie typ obiektu
// **********************************************************************************************************
    if ((token.compare("incfile") == 0) ||
        (token.compare("addfile") == 0) ||
        (token.compare("attachf") == 0))
    { // obs�uga include typowanego
        std::string includetype = readToken(ToLower); // typ obiektu
        std::string includefile = readToken(ToLower); // nazwa pliku
        inctype = includetype;
        incfile = includefile; // coby bylo globalnie widoczne
        WriteLog(AnsiString("INC " + AnsiString(includetype.c_str()) + ", " + includefile.c_str()).c_str());

        QGlobal::asINCLUDETYPE = AnsiString(includetype.c_str());   //Q 010116: trzymamy w globalnej do przypisania objektowi TModel3d
        QGlobal::asINCLUDEFILE = AnsiString(includefile.c_str());

        WriteLog("INCTYPE: " + QGlobal::asINCLUDETYPE);

        AnsiString test = LowerCase(QGlobal::asINCLUDETYPE);

        // Przypisywanie numerycznego identyfikatora dla poszczegolnych typow obiektow
        // MODULE BUILDING SEMAPHOR-S SEMAPHOR-L SIGNALS ROADSIGN HUMAN TRACTION SWITCH TREES-A TREES-B TREES-C GRASS-A GRASS-B HMPOST-A HMPOST-B
        if (test == "module") QGlobal::iINCLUDETYPE = 999;           // modul scenerii z roznymi wpisami
        if (test == "posers") QGlobal::iINCLUDETYPE = 101;           // ludki na peronach, w ogole gdfziekolwiek
        if (test == "semaphor-s") QGlobal::iINCLUDETYPE = 102;       // semafor ksztaltowy
        if (test == "semaphor-l") QGlobal::iINCLUDETYPE = 103;       // semafor swietlny
        if (test == "signals") QGlobal::iINCLUDETYPE = 104;          // inne wskazniki kolejowe
        if (test == "roadsign") QGlobal::iINCLUDETYPE = 105;         // znaki uliczne
        if (test == "traction") QGlobal::iINCLUDETYPE = 106;         // elementy sieci trakcyjnej, np modele slupow
        if (test == "switch") QGlobal::iINCLUDETYPE = 107;           // zwrotnik rozjazdu
        if (test == "trees") QGlobal::iINCLUDETYPE = 108;            // drzewa
        if (test == "hmpost-a") QGlobal::iINCLUDETYPE = 109;         // slupek hektometrowy typ 1
        if (test == "hmpost-b") QGlobal::iINCLUDETYPE = 110;         // slupek hektometrowy typ 2
        

        if (LoadTraction ? true : ((includefile.find("tr/") == std::string::npos) &&
                                   (includefile.find("tra/") == std::string::npos)))
        {
            std::string parameter = readToken(false); // w parametrach nie zmniejszamy (WIELKOSCI ZNAKOW)
            while (parameter.compare("end") != 0)
            {
                par = parameter.c_str();
                tst = par.SubString(1,4);

                if ( tst == "rel-") trainnumber = AnsiString(parameter.c_str()); // WriteLog("TRAINNUMBER " + AnsiString(parameter.c_str()));
                if ( tst == "dst-") stationname = AnsiString(parameter.c_str()); //WriteLog("DESTINATION " + AnsiString(parameter.c_str()));
                if ( tst == "dst-") Global::setpassengerdest(trainnumber, stationname);

                parameters.push_back(parameter);
                parameter = readToken(ToLower);
            }
            // if (trtest2.find("tr/")!=0)
            mIncludeParser = new cParser(includefile, buffer_FILE, mPath, LoadTraction);   // Rozpoczynamy parsowanie...
            if (mIncludeParser->mSize <= 0)
                ErrorLog("Missed include: " + AnsiString(includefile.c_str()));
        }
        else
            while (token.compare("end") != 0)
                token = readToken(ToLower);
        token = readToken(ToLower, Break);
    }
    return token;
}

bool cParser::trimComments(std::string &String)
{
    for (commentmap::iterator cmIt = mComments.begin(); cmIt != mComments.end(); ++cmIt)
    {
        if (String.find((*cmIt).first) != std::string::npos)
        {
            readComment((*cmIt).second);
            String.resize(String.find((*cmIt).first));
            return true;
        }
    }
    return false;
}

std::string cParser::readComment(const std::string Break)
{ // pobieranie znak�w a� do znalezienia znacznika ko�ca
    std::string token = "";
    while (mStream->peek() != EOF)
    { // o ile nie koniec pliku
        token += mStream->get(); // pobranie znaku
        if (token.find(Break) != std::string::npos) // szukanie znacznika ko�ca
            break;
    }
    return token;
}

int cParser::getProgress() const
{
    return mStream->rdbuf()->pubseekoff(0, std::ios_base::cur) * 100 / mSize;
}
