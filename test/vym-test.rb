#!/usr/bin/env ruby

require "#{ENV['PWD']}/scripts/vym-ruby"

$tests_passed = 0
$tests_failed = 0
$tests_total  = 0

def waitkey
  puts "Press return to continue..."
  STDIN.gets
end

def expect (comment, v_exp, v_real)
  if v_exp == v_real
    puts "    Ok: #{comment}"
    $tests_passed += 1
  else  
    puts "Failed: #{comment}. Expected #{v_exp}, but got #{v_real}"
    $tests_failed += 1
  end  
    $tests_total += 1
end

def heading (s)
  puts "\n#{s}\n#{'-' * s.length}\n"
end

def init_map
  # FIXME Missing: check or init default map 
end

def summary
  puts "\nTests done  : #{$tests_total}"
  puts "Tests passed: #{$tests_passed}"
  puts "Tests failed: #{$tests_failed}"
end

vym_mgr=VymManager.new
#vym_mgr.show_running

vym=Vym.new(vym_mgr.find('test') )

#######################
center_0="mc:0"
main_a="mc:0,bo:0"
  branch_a=main_a+",bo:0"
  branch_b=main_a+",bo:1"
  branch_c=main_a+",bo:2"
main_b="mc:0,bo:1"


#######################
heading "Basic checks:"
init_map
vym.select main_a
expect "select", main_a, vym.getSelectString
expect "getHeading", "Main A", vym.getHeading
expect "branchCount", 3, vym.branchCount


#######################
heading "Adding branches:"
init_map
vym.select branch_a
n=vym.branchCount
vym.addBranch()
expect( "addBranch", n+1, vym.branchCount )

vym.undo
expect( "Undo: addBranch", n, vym.branchCount )

init_map
vym.select main_a
n=vym.branchCount
vym.select branch_a
vym.addBranch(-3)
vym.addBranch(-1)
vym.select main_a
expect( "addBranchAbove/Below", n+2, vym.branchCount )

vym.undo
vym.undo
expect( "Undo: addBranchAbove/Below", n, vym.branchCount )

init_map
vym.select branch_a
vym.addBranchBefore
vym.select main_a
expect( "addBranchBefore: check branchcount", n, vym.branchCount)
vym.select branch_a
expect( "addBranchBefore: check heading", "", vym.getHeading)
vym.undo
vym.select main_a
expect( "Undo: addBranchAbove/Below", n, vym.branchCount )

#######################
heading "Adding maps"
init_map
vym.select branch_a
vym.addMapReplace "test/default.vym"
vym.select main_a
expect( "addMapReplace: check branch count in #{main_a}", 3, vym.branchCount )
vym.select branch_a
expect( "addMapReplace: check if #{branch_a} is new", 2, vym.branchCount )

vym.undo
vym.select main_a
expect( "Undo: check branch count in #{main_a}", 3, vym.branchCount )
vym.select branch_a
expect( "Undo: check if #{branch_a} is back", 3, vym.branchCount )

init_map
vym.select main_a
vym.addMapInsert "test/default.vym",1
vym.select main_a
expect( "addMapInsert: branch count", n+1, vym.branchCount )
vym.select main_a + ",bo:1"
expect( "addMapInsert: new heading", vym.getHeading, "MapCenter 0")

vym.undo
vym.select main_a
expect( "Undo: check branch count in #{main_a}", 3, vym.branchCount )
vym.select branch_b
expect( "Undo: check heading of  #{branch_b}", "branch b", vym.getHeading)

#######################
heading "Scrolling and unscrolling"
init_map
vym.select main_a
vym.toggleScroll
expect "toggleScroll", vym.isScrolled, true
vym.undo
expect "undo toggleScroll", vym.isScrolled, false
vym.scroll
expect "scroll", vym.isScrolled, true
vym.unscroll
expect "unscroll", vym.isScrolled, false

init_map
vym.scroll
vym.select branch_a
vym.scroll
vym.select main_a
vym.unscrollChildren
vym.select branch_a
expect "unscrollChildren", vym.isScrolled, false
vym.undo
expect "undo unscrollChildren", vym.isScrolled, true
vym.unscroll
vym.select branch_a
vym.unscroll


#######################
heading "Moving parts"
init_map
vym.select branch_a
vym.moveDown
vym.select branch_a
expect "Moving down", vym.getHeading, "branch b"
vym.undo
vym.select branch_a
expect "Undo Moving down", vym.getHeading, "branch a"

init_map
vym.select branch_b
vym.moveUp
vym.select branch_a
expect "Moving up", vym.getHeading, "branch b"
vym.undo
vym.select branch_b
expect "Undo Moving up", vym.getHeading, "branch b"

init_map
vym.select main_b
n=vym.branchCount
vym.select branch_a
vym.relinkTo main_b,0,0,0
vym.select main_b
expect "RelinkTo #{main_b}: branchCount increased there", n+1, vym.branchCount
vym.undo
vym.select branch_b
expect "Undo: RelinkTo #{main_b}: branchCount decreased there", n, vym.branchCount

init_map
vym.select branch_a
n=vym.branchCount
vym.select main_b
vym.relinkTo branch_a, 1, 0, 0
vym.select branch_a
expect "RelinkTo #{branch_a}, pos 1: branchCount increased there", n+1, vym.branchCount
vym.select "#{branch_a},bo:1"
expect "RelinkTo #{branch_a}, pos 1: Mainbranch really moved", "Main B", vym.getHeading
vym.undo
vym.select center_0
expect "Undo RelinkTo pos 1: branchCount of center", 2, vym.branchCount
# FIXME still has wrong position, check position

summary
exit

# Untested commands:
#
#addSlide
#addXlink
#clearFlags
#colorBranch
#colorSubtree
#copy
#cut
#cycleTask
#delete
#deleteChildren
#deleteKeepChildren
#deleteSlide
#exportAO
#exportASCII
#exportHTML
#exportImage
#exportLaTeX
#exportPDF
#exportPDF
#exportSVG
#exportXML
#importDir
#loadImage
#loadNote
#moveSlideDown
#moveSlideUp
#move
#moveRel
#note2URLs
#paste
#redo
#relinkTo
#saveImage
#saveNote
#selectLastBranch
#selectLastImage
#selectLatestAdded
#setFlag
#setTaskSleep
#setFrameIncludeChildren
#setFrameType
#setFramePenColor
#setFrameBrushColor
#setFramePadding
#setFrameBorderWidth
#setHeading
#setHideExport
#setIncludeImagesHorizontally
#setIncludeImagesVertically
#setHideLinksUnselected
#setMapAuthor
#setMapComment
#setMapBackgroundColor
#setMapDefLinkColor
#setMapLinkStyle
#setNote
#setScale
#setSelectionColor
#setURL
#setVymLink
#sortChildren
#toggleFlag
#toggleFrameIncludeChildren
#toggleTarget
#toggleTask
#unsetFlag