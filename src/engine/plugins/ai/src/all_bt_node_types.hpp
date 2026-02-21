#pragma once
#include "type_list.hpp"
#include "nodes/core_bt_nodes.hpp"
#include "nodes/target_bt_nodes.hpp"
#include "nodes/navmesh_bt_nodes.hpp"
#include "nodes/combat_bt_nodes.hpp"

using AllBTNodeTypes = fsm::Concat_t<
    CoreBTNodeTypes,
    TargetBTNodeTypes,
    NavmeshBTNodeTypes,
    CombatBTNodeTypes
>;
