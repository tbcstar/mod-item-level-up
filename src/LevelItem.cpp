
#include "Chat.h"
#include "Configuration/Config.h"
#include "Player.h"
#include "Creature.h"
#include "AccountMgr.h"
#include "ScriptMgr.h"
#include "Define.h"
#include "GossipDef.h"

uint32 Increase_Level;
uint32 MaxItemLevel = 80;
bool LevelItemEnable = true;
bool LevelItemAnnounce = true;

// Configuration
class mod_levelitem_Conf : public WorldScript
{
public:
    mod_levelitem_Conf() : WorldScript("mod_levelitem_Conf") { }

    // Load Configuration Settings
    void SetInitialWorldSettings()
    {
        LevelItemEnable = sConfigMgr->GetBoolDefault("LevelItem.Enable", true);
        LevelItemAnnounce = sConfigMgr->GetBoolDefault("LevelItem.Announce", true);
        MaxItemLevel = sConfigMgr->GetIntDefault("LevelItem.MaxItemLevel", 80);
    }
};

// Announce
class mod_levelitem_Announce : public PlayerScript
{

public:
    mod_levelitem_Announce() : PlayerScript("mod_levelitem_Announce") {}

    void OnLogin(Player* player) override
    {
        if (sConfigMgr->GetBoolDefault("Arena1v1Announcer.Enable", true))
        {
            ChatHandler(player->GetSession()).SendSysMessage("服务器已启用 |cff4CFF00物品升级 |r模块。");
        }
    }
};

class LevelItem : public ItemScript
{
public:
    LevelItem() : ItemScript("LevelItem") { }

    bool OnUse(Player* p, Item* i, const SpellCastTargets &) override
    {
        if (!LevelItemEnable)
            return false;

        if (p->IsInCombat() || p->IsInFlight() || p->GetMap()->IsBattlegroundOrArena())
        {
            ChatHandler(p->GetSession()).PSendSysMessage("你现在不能用那个!");
            return false;
        }

        if (p->getLevel() >= MaxItemLevel)
        {
            ChatHandler(p->GetSession()).PSendSysMessage("你已经达到等级%u了", MaxItemLevel);
            return false;
        }

        uint8 newLevel = p->getLevel() + 1;
        p->GiveLevel(newLevel);
        p->SetUInt32Value(PLAYER_XP, 0);
        p->DestroyItemCount(i->GetEntry(), 1, true);
        ChatHandler(p->GetSession()).PSendSysMessage("你已经使用了一个升级物品!");

        return true;
    }
};



void AddLevelItemScripts()
{
    new mod_levelitem_Conf();
    new mod_levelitem_Announce();
    new LevelItem();
}
