require 'rake/loaders/makefile'


SOURCE_FILES = FileList['src/**/*.cpp']
OBJECTS = ['shader.o']

task :default => "lib/glutil.a"

def cc_link(target, opts={})
    objects = opts[:objects] || []
    libraries = opts[:libraries] || []
    sh "g++ -std=c++11 -Wall -Wextra -pedantic -o#{target} #{objects.reduce{|s,o| " #{s} #{o} "}} #{libraries.map{|l| "-l#{l}"}.reduce{|s,l| "#{s} #{l} " }}"
end

def cc(source,opts={})
    includeDirs = opts[:includeDirs] || []
    sIncludeDirs = includeDirs.map{|d| "-I#{d}"}.join " "
    sh "g++ -g -std=c++11 #{sIncludeDirs} -Wall -Wextra -pedantic -c #{source}"
end

directory 'lib'

desc "Create glutil library"
file "lib/glutil.a" => OBJECTS + ['lib'] do
    sh "ar crf lib/glutil.a #{OBJECTS.join " "}"
end

desc "Remove temporary files"
task :clean do
    sh "rm *.o .depends.mf"
end

desc "Remove all generated files"
task :clobber do
    Task[:clean].invoke
    sh "rm test"
end

rule ".o" => lambda {|obj| 'src/' + File.basename(obj,'.o') + '.cpp'} do |t|
    cc t.source, :includeDirs => ['include']
end


file ".depends.mf" => SOURCE_FILES do
    sh "g++ -std=c++11 -Iinclude -MM -MF .depends.mf #{SOURCE_FILES.reduce{|s,src| "#{s} #{src}"}}"
end


import ".depends.mf"
