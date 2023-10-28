#include "ChordSheetParser.h"

void ChordSheetParser::ParseChordSheet(const std::string &data, const std::shared_ptr<Song> &song)
{
    if (!song)
        return;

    LOGV_TAG("ChordSheetParser.cpp", "data: %s", data.c_str());

    std::shared_ptr<Instrument> instrument = std::make_shared<Instrument>();

    std::shared_ptr<Staff> staff = std::make_shared<Staff>();

    staff->csStaff = std::make_shared<CSStaff>();
    staff->type = Staff::StaffType::ChordSheet;
    staff->csStaff->measures.emplace_back();
    staff->csStaff->measures.emplace_back();

    instrument->staves.push_back(staff);
    song->instruments.push_back(instrument);

    instrument->systemPositionData.push_back({ 0.0f, 0.0f });
    staff->systemPositionData.push_back({ 0.0f, 0.0f });

    song->songData = SongData();
    InstrumentInfo instrumentInfo;
    instrumentInfo.visible = true;
    song->songData.instrumentInfos.push_back(instrumentInfo);

    std::shared_ptr<System> system = std::make_shared<System>();
    system->systemMeasures.emplace_back();
    system->beginningMeasureIndex = 0;
    system->endingMeasureIndex = 0;

    song->systems.push_back(system);



    Page page = Page();
    song->pages.push_back(page);
    page.pageNumber.SetNumber(1);

    Credit credit;
    credit.words = CreditWords();
    credit.creditType = Credit::CreditType::Title;
    credit.words.text = "Test Song!";
    credit.words.positionX = 500.0f;
    credit.words.positionY = 500.0f;
    song->credits.push_back(credit);
}
