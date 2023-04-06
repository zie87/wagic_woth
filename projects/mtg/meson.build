
wth_srcs = [
    'src/InteractiveButton.cpp',
    'src/AbilityParser.cpp',
    'src/ActionElement.cpp',
    'src/ActionLayer.cpp',
    'src/ActionStack.cpp',
    'src/AIHints.cpp',
    'src/AIMomirPlayer.cpp',
    'src/AIPlayer.cpp',
    'src/AIPlayerBaka.cpp',
    'src/AIStats.cpp',
    'src/AllAbilities.cpp',
    'src/CardGui.cpp',
    'src/CardDescriptor.cpp',
    'src/CardDisplay.cpp',
    'src/CardEffect.cpp',
    'src/CardPrimitive.cpp',
    'src/CardSelector.cpp',
    'src/CardSelectorSingleton.cpp',
    'src/Counters.cpp',
    'src/Credits.cpp',
    'src/Damage.cpp',
    'src/DamagerDamaged.cpp',
    'src/DeckDataWrapper.cpp',
    'src/DeckEditorMenu.cpp',
    'src/DeckMenu.cpp',
    'src/DeckMenuItem.cpp',
    'src/DeckMetaData.cpp',
    'src/DeckStats.cpp',
    'src/DuelLayers.cpp',
    'src/Effects.cpp',
    'src/ExtraCost.cpp',
    'src/GameApp.cpp',
    'src/GameLauncher.cpp',
    'src/GameObserver.cpp',
    'src/GameOptions.cpp',
    'src/GameState.cpp',
    'src/GameStateAwards.cpp',
    'src/GameStateDeckViewer.cpp',
    'src/GameStateDuel.cpp',
    'src/DeckManager.cpp',
    'src/GameStateMenu.cpp',
    'src/GameStateOptions.cpp',
    'src/GameStateShop.cpp',
    'src/GameStateStory.cpp',
    'src/GameStateTransitions.cpp',
    'src/GuiAvatars.cpp',
    'src/GuiBackground.cpp',
    'src/GuiCardsController.cpp',
    'src/GuiCombat.cpp',
    'src/GuiFrame.cpp',
    'src/GuiHand.cpp',
    'src/GuiLayers.cpp',
    'src/GuiMana.cpp',
    'src/GuiPhaseBar.cpp',
    'src/GuiPlay.cpp',
    'src/GuiStatic.cpp',
    'src/IconButton.cpp',
    'src/ManaCost.cpp',
    'src/ManaCostHybrid.cpp',
    'src/MenuItem.cpp',
    'src/ModRules.cpp',
    'src/MTGAbility.cpp',
    'src/MTGCardInstance.cpp',
    'src/MTGCard.cpp',
    'src/MTGDeck.cpp',
    'src/MTGDefinitions.cpp',
    'src/MTGGamePhase.cpp',
    'src/MTGGameZones.cpp',
    'src/MTGPack.cpp',
    'src/MTGRules.cpp',
    'src/Navigator.cpp',
    'src/ObjectAnalytics.cpp',
    'src/OptionItem.cpp',
    'src/PhaseRing.cpp',
    'src/Player.cpp',
    'src/PlayerData.cpp',
    'src/PlayGuiObjectController.cpp',
    'src/PlayGuiObject.cpp',
    'src/PlayRestrictions.cpp',
    'src/Pos.cpp',
    'src/PrecompiledHeader.cpp',
    'src/PriceList.cpp',
    'src/ReplacementEffects.cpp',
    'src/Rules.cpp',
    'src/SimpleMenu.cpp',
    'src/SimpleButton.cpp',
    'src/SimpleMenuItem.cpp',
    'src/SimplePad.cpp',
    'src/SimplePopup.cpp',
    'src/StoryFlow.cpp',
    'src/StyleManager.cpp',
    'src/Subtypes.cpp',
    'src/TargetChooser.cpp',
    'src/TargetsList.cpp',
    'src/TextScroller.cpp',
    'src/ThisDescriptor.cpp',
    'src/Token.cpp',
    'src/Translate.cpp',
    'src/TranslateKeys.cpp',
    'src/Trash.cpp',
    'src/utils.cpp',
    'src/WEvent.cpp',
    'src/WResourceManager.cpp',
    'src/WCachedResource.cpp',
    'src/WDataSrc.cpp',
    'src/WGui.cpp',
    'src/WFilter.cpp',
    'src/Tasks.cpp',
    'src/WFont.cpp',
]

wth_inc_dir = include_directories('./include')
wth_c_args = []
wth_cpp_args = []
wth_link_args = []

if get_option('enable_testsuite')
    wth_cpp_args = wth_cpp_args + ['-DTESTSUITE=1']
    wth_srcs = wth_srcs + ['src/TestSuiteAI.cpp']
endif

if get_option('enable_ai_change_testing')
    wth_cpp_args = wth_cpp_args + ['-DAI_CHANGE_TESTING=1']
endif

if host_machine.system() == 'psp'
    wth_cpp_args = (
        wth_cpp_args + [
            '-DPSPFW3XX',
            '-DDEVHOOK',
            '-DUSE_PRECOMPILED_HEADERS=0',
            '-DPSP',
        ]
    )
    wth_link_args = [
        '-L' + meson.current_source_dir() + '/../../JGE/lib/psp',
        '-lfreetype',
        '-lm',
        '-lmikmod',
        '-lpsppower',
        '-lpspmpeg',
        '-lpspaudiocodec',
        '-lpspaudiolib',
        '-lpspaudio',
        '-lpspmp3',
        '-lpspgum',
        '-lpspgu',
        '-lpsprtc',
        '-lstdc++',
        '-lpspfpu',
        '-lpspdebug',
        '-lpspdisplay',
        '-lpspge',
        '-lpspctrl',
        '-lpspnet',
        '-lpspnet_apctl',
        '-Wl,-zmax-page-size=128',
    ]
else
    wth_cpp_args = wth_cpp_args + ['-DSDL_CONFIG']

    if host_machine.system() == 'linux'
        wth_cpp_args = wth_cpp_args + ['-DLINUX']
    elif host_machine.system() == 'windows'
        wth_cpp_args = wth_cpp_args + ['-DWIN32']
        wth_link_args = (
            wth_link_args + [
                '-lopengl32',
                '-lglu32',
                '-lssp',
            ]
        )
    endif
endif

if get_option('buildtype') == 'debug'
    wth_cpp_args = (
        wth_cpp_args + ['-ggdb3', '-D_DEBUG', '-DDEBUG', '-DDEBUG_CACHE']
    )
endif



wth_name = 'wagic'
#../../projects/mtg/include/PrecompiledHeader.h -o PrecompiledHeader.h.gch  
wth = executable(
    wth_name,
    sources: wth_srcs,
    # cpp_pch: 'include/PrecompiledHeader.h',
    include_directories: [wth_inc_dir],
    dependencies: [jge_dep, hge_dep],
    override_options: ['warning_level=0'],
    cpp_args: wth_cpp_args,
    link_args: wth_link_args,
)

if host_machine.system() == 'psp'
    post_build = meson.get_external_property('post_build_script')
    post_build_script = '@0@'.format(
        find_program(meson.source_root() + '/tools/' + post_build).path(),
    )
    custom_target(
        'wth_post_build',
        input: wth,
        output: 'wth_post_build',
        command: [post_build_script, '@INPUT@'],
        build_by_default: true,
        depends: [wth],
    )
endif
