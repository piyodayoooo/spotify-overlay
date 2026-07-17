# [Spotify Overlay]

Spotifyで再生中の楽曲情報を取得し、歌詞/曲名などを自動表示するデスクトップアプリケーションです。タスクトレイ（システムトレイ）に常駐し、邪魔にならずにオーバーレイ形式でクリックなどの妨げにもなりません。
ゲーム中に曲の歌詞を見ながらやりたい、仕事中に歌いながらしたいなどの場合に使いやすいと思います。

## 特徴 (Features)

* **タスクトレイ（常駐）操作**: アプリはタスクトレイに常駐します。設定の変更、再認証、アプリの終了などはすべて**タスクトレイアイコンを右クリック**して行うシンプルな設計です。
* **Spotify API 連携**: ご自身の Spotify Developer アプリケーションを利用して、現在再生中の楽曲情報をリアルタイムかつ安全に取得します。
* **歌詞の自動取得**: オープンソースの歌詞プロバイダーである [LRCLIB](https://lrclib.net/) を採用し、再生中の楽曲に同期した歌詞データを取得します。

## 事前準備 (Prerequisites)

このアプリを使用するには、ご自身で Spotify Developer ネットワークにアプリを登録し、APIキーを取得する必要があります。

1. [Spotify Developer Dashboard](https://developer.spotify.com/dashboard) にアクセスし、Spotifyアカウントでログインします。
2. 「Create app」をクリックし、新しいアプリケーションを作成します。
3. アプリの設定（Settings）を開き、以下の2つをメモします。
   * **Client ID**
   * **Client Secret**
4. 同じ設定画面の `Redirect URIs` に、 `http://127.0.0.1:8888/callback` を追加して保存します。

## 使い方 (Usage)

1. リリースぺージから最新の実行ファイルをダウンロードし、ソフトを起動します。
2. 事前準備で取得した Spotify の `Client ID` と `Client Secret` を入力して適用します。
3. 初回実行時、ブラウザが立ち上がり Spotify のアクセス許可を求められるので「同意する」をクリックします。
4. Spotify で音楽を再生するとオーバーレイで表示されると思います、位置の変更などの設定はタスクトレイのアイコンからお願いします。

## 貢献 (Contributing)

バグ報告や機能の要望は、[Issues](https://github.com/piyodayoooo/spotify-overlay/issues) にて受け付けています。Pull Request も大歓迎です。
