/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   engineer.h
 * Author: tnc02_000
 *
 * Created on October 31, 2016, 11:37 AM
 */

#ifndef ENGINEER_H
#define ENGINEER_H

enum class engineer {
    elf_hydro, //Elven hydromancer, forests cost reduction, immune to Sea Monster and Storm events
    elf_scout, //Elven scout, forests cost reduction, immune to Sylvan Ambush events
    yeti_cryo,  //Yeti cryomancer, tundra cost reduction, channel cost reduction, immune to Blizzard events
    elf_conjurer, //Elven conjurer, jungle cost reduction, bridge cost reduction
    dwarf_geo, //Dwarven geomancer, hills and mountains cost reduction
    dwarf_trader, //Dwarven trader, hills cost reduction, immune to Sandstorm, Desert Ambush, and Orcish Ambush events
    dwarf_miner, //Dwarven miner, hills cost reduction, rock cost reduction
    drake_arch, //Drake architect, swamp cost reduction, immune to Flood events
    drake_alch, //Drake alchemist, swamp cost reduction, wait one full turn in any city to transform 2 herbs -> 1 potion (TODO: TENTATIVE)
    gnome_acid, //Gnomish acid mole handler, all underground squares cost 2
};

#endif /* ENGINEER_H */

