#ifndef _DOCK_H_
#define _DOCK_H_

#include "imgui.h"
#include "imgui_internal.h"
#include "../Parson/parson.h"

using namespace ImGui;

struct  DockContext;

enum Slot_
{
	Slot_Left,
	Slot_Right,
	Slot_Top,
	Slot_Bottom,
	Slot_Tab,

	Slot_Float,
	Slot_None
};

/*extern void ShutdownDock();
extern void SetNextDock(Slot_ slot);
extern void BeginWorkspace();
extern void EndWorkspace();
extern void SetDockActive();
extern bool BeginDock(const char* label, bool* opened, ImGuiWindowFlags extra_flags);
extern void EndDock();*/
extern DockContext* getDockContext();


struct DockContext
{
	enum EndAction_
	{
		EndAction_None,
		EndAction_Panel,
		EndAction_End,
		EndAction_EndChild
	};


	enum Status_
	{
		Status_Docked,
		Status_Float,
		Status_Dragged
	};


	struct Dock
	{
		Dock();

		~Dock();

		ImVec2 getMinSize() const;
		bool isHorizontal() const;
		void setParent(Dock* dock);
		Dock& getSibling();
		Dock& getFirstTab();
		Dock& getRoot();
		void setActive();
		bool isContainer() const;
		void setChildrenPosSize(const ImVec2& _pos, const ImVec2& _size);
		void setPosSize(const ImVec2& _pos, const ImVec2& _size);

		void LoadSettings(JSON_Object*);
		void LoadHierarchy(JSON_Object*, const DockContext& );
		void Save(JSON_Object*, const DockContext&) const;

		char* label;
		ImU32 id;
		Dock* next_tab;
		Dock* prev_tab;
		Dock* children[2];
		Dock* parent;
		bool active;
		ImVec2 pos;
		ImVec2 size;
		Status_ status;
		char location[16];
		bool opened;
		bool first;
		int last_frame;
		int invalid_frames;
	};


	ImVector<Dock*> m_docks;
	ImVec2 m_drag_offset;
	Dock* m_current = nullptr;
	int m_last_frame = 0;
	EndAction_ m_end_action;
	bool m_is_begin_open = false;

	Dock *m_next_parent;
	ImVec2 m_workspace_pos;
	ImVec2 m_workspace_size;
	Slot_ m_next_dock_slot = Slot_Tab;

	DockContext();
	~DockContext();

	Dock& getDock(const char* label, bool opened);

	void putInBackground();

	void splits();

	void checkNonexistent();

	Dock* getDockAt(const ImVec2& pos) const;

	static ImRect getDockedRect(const ImRect& rect, Slot_ dock_slot);

	static ImRect getSlotRect(ImRect parent_rect, Slot_ dock_slot);

	static ImRect getSlotRectOnBorder(ImRect parent_rect, Slot_ dock_slot);

	Dock* getRootDock();

	bool dockSlots(Dock& dock, Dock* dest_dock, const ImRect& rect, bool on_border);

	void handleDrag(Dock& dock);

	void fillLocation(Dock& dock);

	void doUndock(Dock& dock);

	void drawTabbarListButton(Dock& dock);

	bool tabbar(Dock& dock, bool close_button);

	static void setDockPosSize(Dock& dest, Dock& dock, Slot_ dock_slot, Dock& container);

	void doDock(Dock& dock, Dock* dest, Slot_ dock_slot);

	void rootDock(const ImVec2& pos, const ImVec2& size);

	void setDockActive();

	static Slot_ getSlotFromLocationCode(char code);

	static char getLocationCode(Dock* dock);

	void tryDockToStoredLocation(Dock& dock);

	bool begin(const char* label, bool* opened, ImGuiWindowFlags extra_flags);

	void end();

	int getDockIndex(Dock* dock);

	void ShutdownDock();
	void SetNextDock(Slot_ slot);
	void BeginWorkspace(const char* name);
	void EndWorkspace();
	void SetDockActive();
	bool BeginDock(const char* label, bool* opened, ImGuiWindowFlags extra_flags);
	void EndDock();

	void LoadDock(const JSON_Value*);
	void SaveDocks() const;

	int GetDockPos(const Dock* dock_to_search) const;
	Dock* GetDockbyPos(int pos) const;
	Dock* GetDockbyLabel(const char* name) const;

};

#endif // !_DOCK_H_
