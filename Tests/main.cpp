#include <fstream>
#include <sstream>

#include "gtest/gtest.h"

#include "MultiLocalizationManager.h"

std::string getFirst()
{
	return (std::ostringstream() << std::ifstream("first.txt", std::ios::binary).rdbuf()).str();
}

std::string getSecond()
{
	return (std::ostringstream() << std::ifstream("second.txt", std::ios::binary).rdbuf()).str();
}

TEST(Localization, TextLocalization)
{
	localization::TextLocalization& localization = localization::TextLocalization::get();

	ASSERT_EQ(localization["first"], "First");
	ASSERT_EQ(localization["second"], "Second");

	localization.changeLanguage("ru");

	ASSERT_EQ(localization["first"], getFirst());
	ASSERT_EQ(localization["second"], getSecond());
}

TEST(Localization, MultiLocalizationManager)
{
	localization::MultiLocalizationManager& manager = localization::MultiLocalizationManager::getManager();

	ASSERT_EQ(manager.getLocalizedString("LocalizationData", "first", "en"), "First");
	ASSERT_EQ(manager.getLocalizedString("LocalizationData", "second", "en"), "Second");

	ASSERT_EQ(manager.getLocalizedString("LocalizationData", "first", "ru"), getFirst());
	ASSERT_EQ(manager.getLocalizedString("LocalizationData", "second", "ru"), getSecond());
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}
