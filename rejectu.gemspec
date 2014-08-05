Gem::Specification.new do |s|
  s.name = 'rejectu'
  s.version = '0.0.2'
  s.summary = 'Detects if a UTF-8 string supplementary plane code points'
  s.description = <<-DOC
    This gem detects if a UTF-8 encoded string contains characters from the UTF-8 supplementary
    plane (code points >= U+10000).
  DOC
  s.homepage = 'https://github.com/csfrancis/rejectu'
  s.authors = 'Scott Francis'
  s.email   = 'scott.francis@shopify.com'
  s.license = 'MIT'

  s.files = `git ls-files`.split("\n")
  s.extensions = ['ext/rejectu/extconf.rb']
  s.add_development_dependency 'rake-compiler', '~> 0.9'
end
