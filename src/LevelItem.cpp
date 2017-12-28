#include "Configuration/Config.h"
#include "Player.h"
#include "Creature.h"
#include "AccountMgr.h"
#include "ScriptMgr.h"
#include "Define.h"
#include "GossipDef.h"

uint32 max_lvl;
uint32 Increase_Level;
bool Level_item_enabled;
std::vector<uint32> InvalidMap = { 489, 592, 30, 566, 607, 628, 562, 618, 617, 559, 572 };

class LevelItem : public ItemScript
{
public:
    LevelItem() : ItemScript("LevelItem") { }

    bool OnUse(Player* p, Item* i, const SpellCastTargets &) override
    {
        if (!Level_item_enabled)
            return false;

        if (p->IsInCombat() || p->IsInFlight())
        {
            ChatHandler(p->GetSession()).PSendSysMessage("You can not use this item right now");
            return false;
        }

        if (p->getLevel() == max_lvl)
        {
            ChatHandler(p->GetSession()).PSendSysMessage("You are already Max Level");
            return false;
        }

        for (int i = 0; i < InvalidMap.size(); ++i)
            if (p->GetMapId() == InvalidMap[i])
                return false; 

        uint8 newLevel = p->getLevel() + 1;
        p->GiveLevel(newLevel);
        p->InitTalentForLevel();
        p->SetUInt32Value(PLAYER_XP, 0);
        p->DestroyItemCount(i->GetEntry(), 1, true);
        ChatHandler(p->GetSession()).PSendSysMessage("You have used one level coin");

        return true;
    }
};

class LevelItemConf : public WorldScript
{
public:
    LevelItemConf() : WorldScript("LevelItemConf") { }

    void OnBeforeConfigLoad(bool reload) override
    {
        if (!reload) {
            std::string cfg_file = "LevelItem.conf";
            std::string cfg_def_file = cfg_file + ".dist";

            sConfigMgr->LoadMore(cfg_def_file.c_str());

            sConfigMgr->LoadMore(cfg_file.c_str());

            Level_item_enabled = sConfigMgr->GetBoolDefault("Level_item_enable", true);
            max_lvl = sConfigMgr->GetIntDefault("MaxItemLevel", 80);
        }
    }
};

void AddLevelItemScripts()
{
    new LevelItem();
    new LevelItemConf();
}