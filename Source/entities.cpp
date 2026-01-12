#include "entities.h"

#include <format>

// Local helper func

void RenderFullTextureWrap(const Texture2D& texture, Vector2 position, Vector2 size) {
	const Rectangle sourceRect = { 0, 0, (float)texture.width, (float)texture.height };					// TODO Consider making a get func in res
	const Rectangle destinationRect = { position.x, position.y, size.x, size.y, };
	const Vector2 originOffset = { size.x / 2, size.y / 2 };
	DrawTexturePro(texture, sourceRect, destinationRect, originOffset, 0, WHITE);
}

// PLAYER

Player::Player() noexcept {
	position = Vector2(
		(float)GetScreenWidth() / 2,
		(float)(GetScreenHeight() - player_y_offset)
	);
}

// TODO Move all input checks together in here or in a separate func
// TODO Rename to "move" or similar
void Player::Update() noexcept {
	int direction = IsKeyDown(KEY_RIGHT) - IsKeyDown(KEY_LEFT);
	position.x += speed * direction;

	position.x = Clamp(position.x, 0 + GetRadius(), GetScreenWidth() - GetRadius());
}

void Player::Render(const Resources& res) const noexcept {
	static constexpr float timePerFrame = 0.4f;
	const int i = (int)(GetTime() / timePerFrame) % res.shipTextures.size();
	const auto& frame = res.shipTextures[i];

	static constexpr Vector2 targetSize = { 100, 100 };

	RenderFullTextureWrap(frame, position, targetSize);
}

void Player::OnCollision() noexcept {
	lives -= 1;
}

// PROJECTILE

Projectile::Projectile(Vector2 pos, Vector2 direction, bool fromPlayer) noexcept
	: position(pos)
	, direction(direction)
	, fromPlayer(fromPlayer)
{
}

void Projectile::Update() noexcept { // TODO Rename to "move" or similar. Or move all checks here
	position = Vector2Add(position, direction * speed);

	static constexpr float killHeight = 1500;
	if (position.y < 0.0f || position.y > killHeight) {
		queueDelete = true;
	}
}

void Projectile::Render(const Resources& res) const noexcept {
	static constexpr Vector2 targetSize = { 50, 50 };
	RenderFullTextureWrap(res.laserTexture, position, targetSize);
}

void Projectile::OnCollision() noexcept {
	queueDelete = true;
}

// WALL

Wall::Wall(Vector2 pos) noexcept
	:position(pos)
{
}

void Wall::Render(const Resources& res) const noexcept {
	static constexpr Vector2 targetSize = { 200, 200 };
	RenderFullTextureWrap(res.barrierTexture, position, targetSize);

	// HP label
	static constexpr Vector2 offset = { -21, 10 };
	static constexpr int fontsSize = 40;
	DrawText(std::format("{}", health).c_str(), position.x + offset.x, position.y + offset.y, fontsSize, RED);	// TODO Make a cast-wrapper in common.h
}

void Wall::OnCollision() noexcept {
	health -= 1;
	if (health < 1) {
		queueDelete = true;
	}
}

// ALIEN

Alien::Alien(Vector2 pos) noexcept
	: position(pos)
{
}

void Alien::Update() noexcept {
	position.x += moveRight ? speed : -speed;

	if (position.x <= 0 || position.x >= (float)GetScreenWidth()) {		// TODO Consider a wrapper in comon.h
		position.x = Clamp(position.x, 0, (float)GetScreenWidth());		// TODO Consider a wrapper in comon.h
		position.y += heightChangeOnBorderHit;
		moveRight = !moveRight;
	}
}

void Alien::Render(const Resources& res) const noexcept {
	static constexpr Vector2 targetSize = { 100, 100 };
	RenderFullTextureWrap(res.alienTexture, position, targetSize);
}

void Alien::OnCollision() noexcept {
	queueDelete = true;
}