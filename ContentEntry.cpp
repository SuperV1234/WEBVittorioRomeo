#include "ContentEntry.h"

using namespace std;

string ContentEntry::getOutput()
{
	ctemplate::TemplateDictionary dict("contentEntry");
	dict["ContentEntryTitle"] = title;
	dict["ContentEntryText"] = text;
	dict["ContentEntryTextLeft"] = textLeft;
	dict["ContentEntryTextRight"] = textRight;
	dict["ContentEntryImgLeft"] = imgLeft;
	dict["ContentEntryImgRight"] = imgRight;

	string expanded;
	ctemplate::ExpandTemplate("Templates/" + templateName + ".tpl", ctemplate::DO_NOT_STRIP, &dict, &expanded);

	string result;
	ctemplate::TemplateDictionary dictBase("contentEntryBase");
	dictBase["ContentEntry"] = expanded;
	ctemplate::ExpandTemplate("Templates/contentEntryBase.tpl", ctemplate::DO_NOT_STRIP, &dictBase, &result);

	return result;
}
