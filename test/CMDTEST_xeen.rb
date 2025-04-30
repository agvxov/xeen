class CMDTEST_xeen < Cmdtest::Testcase
  def test_h
    cmd "xeen -h" do
      stdout_equal /.+/
      exit_status 0
    end
  end

  def test_dont_crash
    import_file "test/xl_hw.blob", "./"

    cmd "xeen < xl_hw.blob" do
      created_files ["xeen.png"]
      exit_status 0
    end
  end

  def test_dont_crash2
    import_file "test/bat_hw.blob", "./"

    cmd "xeen < bat_hw.blob" do
      created_files ["xeen.png"]
      exit_status 0
    end
  end
end
