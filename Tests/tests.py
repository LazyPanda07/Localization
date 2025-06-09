import platform
import subprocess
import json
import sys

if __name__ == '__main__':
    executable_path = "../assets/"
    working_dir = "../assets"

    if platform.system() == "Windows":
        executable_path += "Windows/LocalizationUtils.exe"
    else:
        executable_path += "Linux/LocalizationUtils"

    subprocess.run([executable_path, ".", "generate"], cwd=working_dir)

    with open(f"{working_dir}/localization_utils_settings.json", "r") as file:
        settings = json.load(file)

    settings["otherLanguages"] = ["ru"]

    with open(f"{working_dir}/localization_utils_settings.json", "w") as file:
        file.write(json.dumps(settings))

    subprocess.run([executable_path, ".", "generate"], cwd=working_dir)

    en = {
        "first": "First",
        "second": "Second"
    }

    with open(f"{working_dir}/localization/localization_en.json", "w") as file:
        file.write(json.dumps(en))

    subprocess.run([executable_path, ".", "generate"], cwd=working_dir)

    with open(f"{working_dir}/localization/localization_ru.json", "r") as file:
        ru = json.load(file)

    ru["first"] = "Первый"
    ru["second"] = "Второй"

    with open(f"{working_dir}/localization/localization_ru.json", "w", encoding="utf-8") as file:
        file.write(json.dumps(ru, ensure_ascii=False))

    subprocess.run([executable_path, ".", "generate"], cwd=working_dir)

    if sys.argv[1] == "Release":
        subprocess.run([executable_path, ".", "release_build", "../Tests/build/bin"], cwd=working_dir)
    else:
        subprocess.run([executable_path, ".", "debug_build", "../Tests/build/bin"], cwd=working_dir)
