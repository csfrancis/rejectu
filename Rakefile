task :default => :test

# ==========================================================
# Packaging
# ==========================================================

GEMSPEC = eval(File.read('rejectu.gemspec'))

require 'rubygems/package_task'
Gem::PackageTask.new(GEMSPEC) do |pkg|
end

# ==========================================================
# Ruby Extension
# ==========================================================

require 'rake/extensiontask'
Rake::ExtensionTask.new('rejectu', GEMSPEC) do |ext|
  ext.ext_dir = 'ext/rejectu'
  ext.lib_dir = 'lib/rejectu'
end
task :build => :compile

# ==========================================================
# Testing
# ==========================================================

require 'rake/testtask'
Rake::TestTask.new 'test' do |t|
  t.test_files = FileList['test/test_*.rb']
end
task :test => :build
