# 업스트림 기록 (vendored)

이 폴더는 아래 저장소를 **파일로 흡수**한 것이다 (중첩 .git 제거).
업스트림을 따라가려면 아래 정보로 다시 클론해 diff 를 떠서 옮긴다.

```
저장소   https://github.com/Sixze/ALS-Refactored.git
커밋     f1655b790b3ab126a75ec54840486d92d4e28e21
짧은ID   f1655b79
날짜     2026-07-25
브랜치   main
라이선스 MIT (LICENSE.md 유지 필수)
```

## 이 프로젝트가 가한 수정

```diff
diff --git a/Source/ALS/Private/AlsCharacter.cpp b/Source/ALS/Private/AlsCharacter.cpp
index a16b6a25..42420538 100644
--- a/Source/ALS/Private/AlsCharacter.cpp
+++ b/Source/ALS/Private/AlsCharacter.cpp
@@ -395,7 +395,7 @@ void AAlsCharacter::RefreshMeshProperties() const
 		}
 	}
 
-	if (!bMeshIsTicking)
+	if (!bMeshIsTicking && AnimationInstance.IsValid())
 	{
 		AnimationInstance->MarkPendingUpdate();
 	}
```
