/******************************

  This code will:

      *) Get the follower ids of twitterNameOne

      *) Save then to ids.dat

      *) Get the follower ids of twitterNameTwo

      *) Compare them against ids.dat

      *) Write the double-followers to overlap.dat

  It is not robust against errors, etc.

******************************/

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib> //exit
#include <cstring> //strlen
#include <cstdio>  //remove

#include "include/twitcurl.h"

int main( int argc, char* argv[] )
{

  ////// PRELIMS

  std::string userName("yyy");
  std::string passWord("xxxxxxx");

  std::string twitterNameOne("Insert");
  std::string twitterNameTwo("Here");

  twitCurl twitterObj;

  twitterObj.setTwitterUsername( userName );
  twitterObj.setTwitterPassword( passWord );

  twitterObj.getOAuth().setConsumerKey( std::string("ej3NYeeyT9ONYLGDzSSIZbzDw") );
  twitterObj.getOAuth().setConsumerSecret( std::string("tjHqzh6RRRYdT0sv2dkg7bxQG2jNWh5jSYvvOFeVmbYk6a6Ltt") );

  std::string authUrl;
  twitterObj.oAuthRequestToken( authUrl );
  twitterObj.oAuthHandlePIN( authUrl );
  twitterObj.oAuthAccessToken();

  std::string myOAuthAccessTokenKey;
  std::string myOAuthAccessTokenSecret;
  twitterObj.getOAuth().getOAuthTokenKey( myOAuthAccessTokenKey );
  twitterObj.getOAuth().getOAuthTokenSecret( myOAuthAccessTokenSecret );

  std::string webIds;
  std::string webResponse;
  std::string nextCursor;

  std::fstream idsFile("ids.dat",std::fstream::in|std::fstream::out|std::fstream::app);
  std::cout << std::endl;

  ////// LOOP 1

  int page = 0;
  int requests = 0;

  nextCursor = "";
  //nextCursor = "1448072076781313873";
  //nextCursor = "0";

  while( nextCursor.compare("0")!=0 )
  {

    page++;
    requests++;
    if(requests>15)
    {
      std::cout << "That's it for now... restart in 15 min with cursor: " << nextCursor << std::endl;
      exit(0);
    }

    std::cout << "Getting " << twitterNameOne << "page #" << page << " with cursor: " << nextCursor << std::endl;

    twitterObj.followersIdsGet( nextCursor, twitterNameOne );
    twitterObj.getLastWebResponse( webResponse );

    size_t pos    = webResponse.find("ids");
           pos   += strlen("ids\":[");
           webIds = webResponse.substr(pos);
           pos    = webIds.find("]");
           webIds = webIds.substr(0,pos);

    idsFile << webIds << ",";

    pos        = webResponse.find("next_cursor");
    pos       += strlen("next_cursor\":");
    nextCursor = webResponse.substr(pos);
    pos        = nextCursor.find(",");
    nextCursor = nextCursor.substr(0,pos);

  } 

  ////// LOOP 2

  std::fstream overlapFile("overlap.dat",std::fstream::out);

  page=0;
  int count = 0;
  nextCursor = "";

  while( nextCursor.compare("0")!=0 )
  {

    page++;
    requests++;

    if(requests>15)
    {
      std::cout << std::endl;
      std::cout << "That's it for now... restart in 15 min with cursor: " << nextCursor << std::endl;
      exit(0);
    }

    std::cout << "Getting " << twitterNameTwo << " page #" << page << " with cursor: " << nextCursor << std::endl;

    twitterObj.followersIdsGet( nextCursor, twitterNameTwo );
    twitterObj.getLastWebResponse( webResponse );

    size_t pos    = webResponse.find("ids");
           pos   += strlen("ids\":[");
           webIds = webResponse.substr(pos);
           pos    = webIds.find("]");
           webIds = webIds.substr(0,pos);

    long long int idOne,idTwo;
    bool flag = false;

    while(1)
    {

      idTwo = std::stoll(webIds,&pos);

      idsFile.clear();
      idsFile.seekg(0);
      while( idsFile >> idOne )
      {

        if( idOne == idTwo)
        {
          flag=true;
          overlapFile << idTwo << " ";
          std::cout << "\r  Found: " << ++count;
          std::cout.flush();
          break;
        }
        idsFile.ignore();
      }

      webIds = webIds.substr(pos);

      if( webIds.size() == 0 )
        break;

      webIds = webIds.substr(strlen(","));

    }

    if(flag)
      std::cout << std::endl;

    pos        = webResponse.find("next_cursor");
    pos       += strlen("next_cursor\":");
    nextCursor = webResponse.substr(pos);
    pos        = nextCursor.find(",");
    nextCursor = nextCursor.substr(0,pos);

  }

  overlapFile.close();

  ////// FINISH

  idsFile.close();
  //std::remove("ids.dat");

}








