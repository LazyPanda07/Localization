#include "gtest/gtest.h"

#include "MultiLocalizationManager.h"

TEST(Localization, TextLocalization)
{
	localization::TextLocalization& localization = localization::TextLocalization::get();

	ASSERT_EQ(localization["first"], "First");
	ASSERT_EQ(localization["second"], "Second");

	localization.changeLanguage("ru");

	ASSERT_EQ(localization["first"], "������");
	ASSERT_EQ(localization["second"], "������");
}

TEST(Localization, MultiLocalizationManager)
{
	localization::MultiLocalizationManager& manager = localization::MultiLocalizationManager::getManager();

	ASSERT_EQ(manager.getLocalizedString("LocalizationData", "first"), "First");
	ASSERT_EQ(manager.getLocalizedString("LocalizationData", "second", "en"), "Second");

	ASSERT_EQ(manager.getLocalizedString("LocalizationData", "first", "ru"), "������");
	ASSERT_EQ(manager.getLocalizedString("LocalizationData", "second", "ru"), "������");
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}
