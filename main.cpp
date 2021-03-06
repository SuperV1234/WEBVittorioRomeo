#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <ctemplate/template.h>
#include <json/json.h>
#include "Utils.h"
#include "Content.h"
#include "ContentEntry.h"
#include "Header.h"
#include "MenuItem.h"
#include "Global.h"
#include <boost/filesystem.hpp>

using namespace std;
using namespace boost::filesystem;

string getContentEntriesOutput(string mPath)
{
	string contentEntriesOutput;
	vector<ContentEntry> contentEntries;
	Json::Value root{getJsonFileRoot(mPath + "contentEntries.json")};
	for(auto value : root["entries"])
	{
		string sTemplate{value["template"].asString()};
		string title	{value["title"].asString()};
		string text		{value["text"].asString()};
		string textLeft	{value["textLeft"].asString()};
		string textRight{value["textRight"].asString()};
		string imgLeft	{value["imgLeft"].asString()};
		string imgRight	{value["imgRight"].asString()};
		string html		{value["html"].asString()};
		string menu		{value["menu"].asString()};
		string image	{value["image"].asString()};

		contentEntries.push_back({sTemplate, title, text, textLeft, textRight, imgLeft, imgRight, html, menu, image});
	}
	for(auto contentEntry : contentEntries) contentEntriesOutput += contentEntry.getOutput() += "\n";
	return contentEntriesOutput;
}

string getMenuItemsOutput(string mPath)
{
	string menuItemsOutput;
	vector<MenuItem> menuItems;
	Json::Value root{getJsonFileRoot(mPath)};
	for(auto value : root["items"])
	{
		string additional{""};
		if(!value[2].empty()) additional = value[2].asString();

		menuItems.push_back({value[0].asString(), value[1].asString(), additional});
	}
	for(auto menuItem : menuItems) menuItemsOutput += menuItem.getOutput() += "\n";
	return menuItemsOutput;
}

int main()
{
	cout << "Loading pages" << endl << endl;
	vector<path> pageDirectoryVector;
	string pagesPath{"Json/Pages/"};
	recursiveDirectoryFill(pageDirectoryVector, path(pagesPath));

	for(auto directoryPath : pageDirectoryVector)
	{
		string folderName{directoryPath.string().substr(pagesPath.size())};
		string jsonFilePath{pagesPath + folderName + "/page.json"};

		cout << "Trying JSON root: " << jsonFilePath << endl;
		Json::Value root{getJsonFileRoot(jsonFilePath)};

		string id		{folderName};
		string name		{root["name"].asString()};

		Page page{id, name};

		cout << "Adding menus to page: ";
		for(auto menu : root["menus"])
		{
			cout << menu.asString() << ", ";
			page.menus.push_back(menu.asString());
		}

		getPageMap().insert(make_pair(id, page));
		
		cout << endl << endl;
	}

	cout << endl << endl;

	//______________________________________________

	cout << "Loading menus" << endl << endl;
	string menusPath{"Json/Menus/"};

	for(auto filePath : getAllFilePaths(menusPath, ".json"))
	{
		cout << "Trying JSON root: " << filePath << endl;
		Json::Value root{getJsonFileRoot(filePath)};
		
		string id		{root["id"].asString()};

		getMenuMap().insert(make_pair(id, Menu{id, getMenuItemsOutput(filePath)}));
	}

	cout << endl << endl << endl;
	
	//______________________________________________

	cout << "Parsing pages" << endl << endl;

	string headerOutput{Header{}.getOutput()};
	for(auto pagePair : getPageMap())
	{
		Page page{pagePair.second};

		string resultFilePath{"Result/" + page.id + ".html"};
		string folderPath{pagesPath + page.id + "/"};

		page.headerOutput = headerOutput;

		for(auto menu : page.menus) page.menuOutput += getMenuMap()[menu].getOutput();

		cout << "Getting page content: " << folderPath << endl;
		page.contentOutput = Content{page.name, getContentEntriesOutput(folderPath)}.getOutput();

		path parentPath{path{resultFilePath}.remove_leaf()};
		cout << "Checking if path exists: " << parentPath.string() << endl;
		if(!exists(parentPath)) create_directory(parentPath);

		cout << "Writing page to file: " << resultFilePath << endl;
		ofstream o{resultFilePath};
		o << page.getOutput();
		o.flush();
		o.close();
		
		cout << endl;
	}

    return 0;
}
